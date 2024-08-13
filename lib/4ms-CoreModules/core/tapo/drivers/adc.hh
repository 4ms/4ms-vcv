// Copyright 2016 Matthias Puech.
//
// Author: Matthias Puech <matthias.puech@gmail.com>
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
// Driver for ADC.

#pragma once

#include "../stmlib/stmlib.h"
#include <array>

namespace TapoDelay {

enum AdcChannel {
  // pots
  ADC_SCALE_POT,
  ADC_FEEDBACK_POT,
  ADC_MODULATION_POT,
  ADC_DRYWET_POT,
  ADC_MORPH_POT,
  ADC_GAIN_POT,

  // bipolar CV
  ADC_SCALE_CV,
  ADC_FEEDBACK_CV,
  ADC_MODULATION_CV,
  ADC_DRYWET_CV,

  // unipolar CV
  ADC_CLOCK_CV,
  ADC_FSR_CV,
  ADC_VEL_CV,
  ADC_TAPTRIG_CV,
  ADC_CHANNEL_LAST
};

struct Adc {
  void Deinit() {}
  void Init() {
    std::fill(values_.begin(), values_.end(), 0.0f);
  }
  void Convert() {}
  void Wait() {}

  inline void set(uint8_t channel, float val)
  {
    values_[channel] = val;
  }

  inline uint16_t value(uint8_t channel) const {
    return uint16_t(float_value(channel) * 65535.0f);
  }
  inline float float_value(uint8_t channel) const {
    return values_[channel];
  }

 private:
  std::array<float,ADC_CHANNEL_LAST> values_;
};

}
