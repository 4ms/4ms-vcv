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
// Tap allocator

#pragma once

#include "tap.hh"
#include "stmlib/utils/ring_buffer.h"

namespace TapoDelay {

class TapAllocator
{
 public:
  void Init(Tap taps[kMaxTaps]);
  bool Add(float time, float velocity, VelocityType velocity_type, float pan);
  void RemoveFirst();
  bool RemoveLast();
  void Clear();
  void Poll();

  void Save(Slot* slot);
  void Load(Slot* slot);

  void set_fade_time(float fade_time) {
    fade_time_ = fade_time;
  }

  float total_volumes() {
    float sum = 0.0f;
    for(int i=0; i<kMaxTaps; i++) {
      sum += taps_[i].volume();
    }
    return sum;
  }

  float max_time() { return max_time_; }

 private:
  // the pool is writeable if it is not full, and if the last voice
  // has finished fading out
  bool writeable() { return !full() && !taps_[next_voice_].active(); };
  bool empty() { return count_voices_ == 0; }
  bool full() { return count_voices_ == kMaxTaps; }
  uint8_t count_voices() { return count_voices_; }
  void RecomputeMaxTime();
  bool Add(bool loading, float time, float velocity, VelocityType velocity_type, float pan);

  Tap* taps_;

  int8_t next_voice_{};
  int8_t oldest_voice_{};
  int8_t count_voices_{};
  float fade_time_{};
  float max_time_{};

  stmlib::RingBuffer<TapParameters, kMaxTaps*4> queue_{};
};

}
