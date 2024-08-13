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

#pragma once

#include "parameters.hh"
#include "tap_allocator.hh"
#include "stmlib/utils/observer.h"
#include "average.hh"

#include "stmlib/dsp/filter.h"

namespace TapoDelay {

using namespace stmlib;

enum TapType {
  TAP_DRY,
  TAP_ADDED,
  TAP_ADDED_OVERWRITE,
  TAP_ADDED_OVERDUB,
  TAP_CROSSED,
  TAP_FAIL,
};

class MultitapDelay
{
public:
  void Init(short* buffer, int32_t buffer_size, uint32_t sample_rate);
  void Process(Parameters *params, ShortFrame* input, ShortFrame* output);

  void changeSampleRate(uint32_t);

  void AddTap(Parameters *params);
  void Clear();
  void RemoveLastTap();
  void RepanTaps(PanningMode panning_mode);
  void ClockTick();

  void set_repeat(bool state);
  void set_sync(bool state);

  void Load(Slot* slot) {
    tap_allocator_.Load(slot);
    if (slot->size > 0) {
      counter_running_ = true;
    } else { // bank IR
      counter_ = 0;
      counter_running_ = false;
    }
  };

  void Save(Slot* slot) {
    tap_allocator_.Save(slot);
  };

  void Poll() { tap_allocator_.Poll(); }

  bool counter_running() { return counter_running_; }
  float repeat() { return repeat_fader_.volume(); }
  bool sync() { return sync_; }
  bool quantize() { return quantize_; }

  void sequencer_step(float morph_time) {
    step_observable_.notify(morph_time);
  }

  size_t buffer_size() { return buffer_.size(); }

  Observable0 reset_observable_{};
  Observable0 slot_modified_observable_{};
  Observable0 tap_modulo_observable_{};
  Observable2<TapType, float> tap_observable_{};
  Observable1<float> step_observable_{};

private:
  template<bool repeat_tap_on_output>
  void Process(Parameters *params, ShortFrame* input, ShortFrame* output);
  float ComputePanning(PanningMode panning_mode);

  TapAllocator tap_allocator_{};
  Tap taps_[kMaxTaps]{};
  AudioBuffer buffer_{};
  float feedback_buffer_[kBlockSize]{};
  float feedback_compensation_{};
  Svf dc_blocker_{};
  Fader repeat_fader_{};
  uint32_t counter_{};

  uint32_t repeat_time_{};
  uint32_t clock_counter_{};
  Average<4> clock_period_{};
  float clock_period_smoothed_{};
  float sync_scale_{};

  bool sync_{};
  bool counter_running_{};

  bool quantize_{};

  bool pan_state_{};

  Parameters prev_params_{};
  uint32_t prev_max_time_{};

  uint32_t kMaxQuantizeClock{};
};

}

