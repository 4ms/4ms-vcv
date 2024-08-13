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
//  A single tap

#pragma once

#include "stmlib/dsp/dsp.h"
#include "stmlib/dsp/filter.h"
#include "stmlib/dsp/units.h"
#include "stmlib/dsp/rsqrt.h"

#include "parameters.hh"
#include "audio_buffer.hh"
#include "random_oscillator.hh"
#include "fader.hh"

namespace TapoDelay {

using namespace stmlib;

const float kTimeLfoAmplitude = 0.5f;
// Warning: at high frequency and low Q, filter becomes unstable
const float kCutoffLowestNote = 30.0f;
const float kCutoffNrOctaves = 7.689f;

class Tap
{
 public:

  void Init(uint32_t sample_rate) {
    lfo_.Init();
    fader_.Init();
    previous_lfo_sample_ = 0.0f;
    time_ = kBlockSize;
    velocity_ = 0.0f;
    panning_ = 0.5f;
    kSampleRate = sample_rate;
  };

  /* minimum time is block size */
  inline void set_time(float time) { time_ = time; }
  inline void set_velocity(float velocity, VelocityType velo_type) {
    velocity_ = velocity;
    velocity_type_ = velo_type;
  }

  inline void set_panning(float panning) {
    panning_ = panning;
  }

  float time() { return time_; }
  float velocity() { return velocity_; }
  VelocityType velocity_type() { return velocity_type_; }
  float panning() { return panning_; }

  bool active() { return fader_.active(); }
  float volume() { return fader_.volume(); }
  void fade_in(float length) { fader_.fade_in(length); }
  void fade_out(float length) { fader_.fade_out(length); }

  /* Dispatch function */
  void Process(Parameters *prev_params, Parameters *params,
               AudioBuffer *buffer, FloatFrame* output) {
    if (velocity_type_ == VELOCITY_AMP)
      Process<VELOCITY_AMP>(prev_params, params, buffer, output);
    else if (velocity_type_ == VELOCITY_LP)
      Process<VELOCITY_LP>(prev_params, params, buffer, output);
    else if (velocity_type_ == VELOCITY_BP)
      Process<VELOCITY_BP>(prev_params, params, buffer, output);
  }

  template<VelocityType velocity_type>
  void Process(Parameters *prev_params, Parameters *params,
               AudioBuffer *buffer, FloatFrame* output) {

    float velocity = velocity_;

    const float kA440 = 440.0f / kSampleRate;

    /* set filter parameters */
    if (velocity_type == VELOCITY_LP) {
      velocity *= 1.0f - params->velocity_parameter;
      velocity += params->velocity_parameter;
      velocity *= velocity;
    } else if (velocity_type == VELOCITY_BP) {
      float f = SemitonesToRatio(velocity * 12.0f * kCutoffNrOctaves
                                 - 69.0f + kCutoffLowestNote) * kA440;
      float q = params->velocity_parameter * params->velocity_parameter * 20.0f + 1.0f;
      filter_.set_f_q<FREQUENCY_FAST>(f, q);
    } else if (velocity_type == VELOCITY_AMP) {
      velocity *= 1.0f - params->velocity_parameter;
      velocity += params->velocity_parameter;
      velocity *= velocity * velocity;
    }

    /* compute random LFO */
    lfo_.set_slope(params->modulation_frequency);
    float lfo_sample = lfo_.Next(); // -1..1

    // min time is kBlockSize
    float time_start = time_ * prev_params->scale + kBlockSize;
    float time_end = time_ * params->scale + kBlockSize;

    float amplitude_start = kTimeLfoAmplitude * kSampleRate;
    float amplitude_end = kTimeLfoAmplitude * kSampleRate;

    // limit LFO amplitude to no cross write head
    if (amplitude_start >= time_start - kBlockSize) {
      amplitude_start = time_start - kBlockSize;
    }
    if (amplitude_end >= time_end - kBlockSize) {
      amplitude_end = time_end - kBlockSize;
    }

    time_start += amplitude_start * previous_lfo_sample_ * prev_params->modulation_amount;
    time_end += amplitude_end * lfo_sample * params->modulation_amount;
    previous_lfo_sample_ = lfo_sample;

    float time = 0.0f;
    const float time_increment = (time_end - time_start - kBlockSize)
      / static_cast<float>(kBlockSize);

    fader_.Prepare();

    size_t size = kBlockSize;
    while(size--) {

      /* read sample from buffer */
      /* NOTE: doing the addition here avoids rounding errors with large times */
      float sample = buffer->ReadLinear(time_start + time);

      union {float f; int i;} t; t.i = time_;
      sample = (t.i & 1) ? sample : -sample;

      /* apply envelope */
      fader_.Process(sample);

      /* apply velocity */
      if (velocity_type == VELOCITY_AMP) {
        sample *= velocity;
      } else if (velocity_type == VELOCITY_LP) {
        ONE_POLE(lp_filter1_, sample, velocity);
        ONE_POLE(lp_filter2_, lp_filter1_, velocity);
        ONE_POLE(lp_filter3_, lp_filter2_, velocity);
        sample = lp_filter3_;
      } else if (velocity_type == VELOCITY_BP) {
        sample = filter_.Process<FILTER_MODE_BAND_PASS>(sample);
        sample *= fast_rsqrt_carmack(params->velocity_parameter * params->velocity_parameter * 20.0f + 1.0f);
      }

      /* write to buffer */
      output->l += sample * panning_;
      output->r += sample * (1.0f - panning_);

      /* increment stuff */
      output++;
      time += time_increment;
    }
  };

 private:

  Svf filter_{};
  float lp_filter1_{}, lp_filter2_{}, lp_filter3_{};

  VelocityType velocity_type_{};
  float time_{};
  float velocity_{};
  float panning_{};

  Fader fader_{};

  RandomOscillator lfo_{};
  float previous_lfo_sample_{};

  uint32_t kSampleRate{};

};

}


