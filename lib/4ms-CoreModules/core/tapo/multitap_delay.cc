// Copyright 2015 Matthias Puech.
//
// Author: Matthias Puech (matthias.puech@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Multitap delay

#include "stmlib/dsp/dsp.h"
#include "stmlib/dsp/parameter_interpolator.h"
#include "stmlib/dsp/rsqrt.h"
#include "multitap_delay.hh"

namespace TapoDelay {

using namespace stmlib;



void MultitapDelay::Init(short* buffer, int32_t buffer_size, const uint32_t sample_rate) {

  const int32_t kClockDefaultPeriod = 1 * sample_rate;
  kMaxQuantizeClock = 2 * sample_rate;

  buffer_.Init(buffer, buffer_size);
  dc_blocker_.Init();
  dc_blocker_.set_f_q<FREQUENCY_FAST>(10.0f / sample_rate, 0.6f);
  repeat_fader_.Init();
  clock_period_.Init(kClockDefaultPeriod);
  clock_period_smoothed_ = kClockDefaultPeriod;
  sync_scale_ = kClockDefaultPeriod;
  quantize_ = false;

  for (size_t i=0; i<kMaxTaps; i++) {
    taps_[i].Init(sample_rate);
  }
  tap_allocator_.Init(taps_);

  buffer_.Clear();
};

void MultitapDelay::changeSampleRate(uint32_t sample_rate)
{
  const int32_t kClockDefaultPeriod = 1 * sample_rate;
  clock_period_.Init(kClockDefaultPeriod);
  clock_period_smoothed_ = kClockDefaultPeriod;
  sync_scale_ = kClockDefaultPeriod;

  kMaxQuantizeClock = 2 * sample_rate;

  for (size_t i=0; i<kMaxTaps; i++) {
    taps_[i].Init(sample_rate);
  }
  tap_allocator_.Init(taps_);
}

void MultitapDelay::set_repeat(bool state) {
  if (state) {
    repeat_fader_.fade_in(prev_params_.morph + 1.0f);
    // sample repeat time
    float repeat_time = tap_allocator_.max_time() * prev_params_.scale;
    repeat_time_ = static_cast<uint32_t>(repeat_time);
  } else {
    repeat_fader_.fade_out(prev_params_.morph);
  }
}

void MultitapDelay::set_sync(bool state) {
  if (state) {
    quantize_ = false;
  }
  sync_ = state;
}


float MultitapDelay::ComputePanning(PanningMode panning_mode)
{
    float panning = 1.0f;
    if (panning_mode == PANNING_RANDOM) {
      panning = Random::GetFloat();
    } else if (panning_mode == PANNING_ALTERNATE) {
      panning = pan_state_ ? 1.0f : 0.0f;
      pan_state_ = !pan_state_;
    }

    return panning;
}

void MultitapDelay::ClockTick() {
  if (clock_counter_ < kMaxQuantizeClock &&
      !sync_) {
    quantize_ = true;
  }

  clock_period_.Process(clock_counter_);
  clock_counter_ = 0;
}

void MultitapDelay::AddTap(Parameters *params) {

  // inhibit tap add completely
  if (params->edit_mode == EDIT_OFF) {
    return;
  }

  // first tap does not count, it just starts the counter
  if (!counter_running_) {
    counter_running_ = true;
    tap_observable_.notify(TAP_DRY, 1.0f);
    return;
  }

  float counter = static_cast<float>(counter_);

  if (quantize_) {
    float period = static_cast<float>(clock_period_.value());
    counter = floorf(counter / period + 0.5f) * period;
  }

  float time = counter / prev_params_.scale;
  float pan = ComputePanning(params->panning_mode);

  TapType type =
    params->edit_mode == EDIT_NORMAL ||
    // if it's the first tap, then notify it as "normal"
    tap_allocator_.max_time() == 0.0f ? TAP_ADDED :
    params->edit_mode == EDIT_OVERDUB ? TAP_ADDED_OVERDUB :
    TAP_FAIL;

  // add tap
  bool success = false;
  if (time < buffer_.size()) {
    success = tap_allocator_.Add(time,
                                 params->velocity,
                                 params->velocity_type,
                                 pan);
  }

  // if we ran out of taps
  if (!success) type = TAP_ADDED_OVERWRITE;

  // UI feedback
  tap_observable_.notify(type, params->velocity);
  slot_modified_observable_.notify();
}

void MultitapDelay::RemoveLastTap() {
  if (tap_allocator_.RemoveLast()) {
    slot_modified_observable_.notify();
  }

  if(tap_allocator_.max_time() <= 0.0f) {
    counter_running_ = false;
    counter_ = 0;
  }
}

void MultitapDelay::Clear() {
  tap_allocator_.Clear();
  counter_running_ = false;
  counter_ = 0;
  slot_modified_observable_.notify();
}

void MultitapDelay::RepanTaps(PanningMode panning_mode) {
  for (int i=0; i<kMaxTaps; i++) {
    float pan = ComputePanning(panning_mode);
    taps_[i].set_panning(pan);
  }
}

// Dispatch
void MultitapDelay::Process(Parameters *params, ShortFrame* input, ShortFrame* output) {
  return params->panning_mode == PANNING_LEFT ?
     Process<true>(params, input, output) :
     Process<false>(params, input, output);
}

template<bool last_tap_on_output>
void MultitapDelay::Process(Parameters *params, ShortFrame* input, ShortFrame* output) {

  static const float buffer_headroom = 0.5f;

  // compute IR scale to fit into clock period
  if (sync_ && tap_allocator_.max_time() > 0.0f) {
    ONE_POLE(clock_period_smoothed_, clock_period_.value(), 0.002f);
    params->scale = clock_period_smoothed_
      / tap_allocator_.max_time()
      * params->sync_ratio; // warning: overwriting a parameter
  }

  uint32_t max_time = static_cast<uint32_t>(tap_allocator_.max_time() * params->scale);

  // increment sample counter
  if (counter_running_) {
    counter_ += kBlockSize;
    // in the right edit modes, reset counter
    if ((params->edit_mode != EDIT_NORMAL || sync_) &&
        counter_ > max_time) {
      counter_ -= max_time;
    }
  }

  // set fade time
  tap_allocator_.set_fade_time(params->morph);

  /* 1. Write (dry+repeat+feedback) to buffer */

  float gain = prev_params_.gain;
  float gain_end = params->gain;
  float gain_increment = (gain_end - gain) / kBlockSize;

  float feedback_compensation = static_cast<float>(tap_allocator_.total_volumes());
  if (feedback_compensation < 1.0f) feedback_compensation = 1.0f;
  feedback_compensation = fast_rsqrt_carmack(feedback_compensation);
  ONE_POLE(feedback_compensation_, feedback_compensation, 0.05f);
  params->feedback *= feedback_compensation_; // warning: overwrite params

  float feedback = prev_params_.feedback;
  float feedback_end = params->feedback;
  float feedback_increment = (feedback_end - feedback) / kBlockSize;

  for (size_t i=0; i<kBlockSize; i++) {
    float fb_sample = feedback_buffer_[i];
    int16_t r = buffer_.ReadShort(repeat_time_);
    float repeat_sample =  static_cast<float>(r) / 32768.0f / buffer_headroom;
    repeat_fader_.Process(repeat_sample);
    float dry_sample = static_cast<float>(input[i].l) / 32768.0f;
    float dither = (Random::GetFloat() - 0.5f) / 8192.0f;
    float s = gain * dry_sample + feedback * fb_sample + repeat_sample + dither;
    s = SoftLimit(s * buffer_headroom);
    int16_t sample = Clip16(static_cast<int32_t>(s * 32768.0f));
    repeat_fader_.Prepare();
    buffer_.Write(sample);
    gain += gain_increment;
    feedback += feedback_increment;
  }

  /* 2. Read and sum taps from buffer */

  FloatFrame buf[kBlockSize];
  FloatFrame empty = {0.0f, 0.0f};
  std::fill(buf, buf+kBlockSize, empty);

  uint32_t counter_modulo = max_time ? counter_ % max_time : counter_;

  bool counter_modulo_reset = counter_running_ && counter_modulo < kBlockSize+1;
  float counter_on_tap = 0.0f;
  bool counter_modulo_on_tap = false;

  for (int i=0; i<kMaxTaps; i++) {
    taps_[i].Process(&prev_params_, params, &buffer_, buf);

    float time = taps_[i].time() * params->scale;
    if (counter_running_ && taps_[i].active()) {
      if (time-kBlockSize <= counter_modulo && counter_modulo < time+kBlockSize) {
        counter_modulo_on_tap = true;
      }
      if (time-kBlockSize <= counter_ && counter_ < time+kBlockSize) {
        counter_on_tap = taps_[i].velocity();
      }
    }
  }

  // notify UI of tap
  if (counter_modulo_reset) {
    reset_observable_.notify();
  }

  if (counter_on_tap > 0.0f) {
    tap_observable_.notify(TAP_CROSSED, counter_on_tap);
  }

  if (counter_modulo_on_tap) {
    tap_modulo_observable_.notify();
  }

  /* 3. Feed back, apply dry/wet, write to output */

  float drywet = prev_params_.drywet;
  float drywet_end = params->drywet;
  float drywet_increment = (drywet_end - drywet) / kBlockSize;

  float max_time_index = prev_max_time_ + kBlockSize;
  float max_time_index_end = max_time;
  float max_time_index_increment = (max_time_index_end - max_time_index) / kBlockSize;

  /* convert, output and feed back */
  for (size_t i=0; i<kBlockSize; i++) {
    FloatFrame sample = { buf[i].l / buffer_headroom,
                          buf[i].r / buffer_headroom };

    // write to feedback buffer
    float fb = sample.l + sample.r;
    feedback_buffer_[i] = dc_blocker_.Process<FILTER_MODE_HIGH_PASS>(fb);

    // add dry signal
    float dry = static_cast<float>(input[i].l) / 32768.0f;
    float fade_in = Interpolate(lut_xfade_in, drywet, 16.0f);
    float fade_out = Interpolate(lut_xfade_out, drywet, 16.0f);
    sample.l = dry * fade_out + sample.l * fade_in;

    // write to output buffer
    if (last_tap_on_output) {
      sample.r = buffer_.ReadHermite(max_time_index) / buffer_headroom;
    } else {
      sample.r = dry * fade_out + sample.r * fade_in;
    }

    output[i].l = SoftConvert(sample.l);
    output[i].r = SoftConvert(sample.r);

    drywet += drywet_increment;
    max_time_index += max_time_index_increment;
  }

  prev_max_time_ = max_time;
  prev_params_ = *params;
  clock_counter_ += kBlockSize;

  if (clock_counter_ > kMaxQuantizeClock) {
    quantize_ = false;
  }
};

}