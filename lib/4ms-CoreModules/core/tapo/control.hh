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
// Calibration settings.

#pragma once

#include "stmlib/stmlib.h"
#include "stmlib/dsp/filter.h"
#include "average.hh"

#include "drivers/adc.hh"
#include "drivers/gate_input.hh"
#include "parameters.hh"
#include "persistent.hh"
#include "multitap_delay.hh"

namespace TapoDelay {

using namespace stmlib;

class Control {
 public:
  void Init(MultitapDelay* delay, CalibrationData* calibration_data);
  void Read(Parameters* parameters, bool sequencer_mode);
  void Calibrate();

  Adc& getADCDriver()
  {
    return adc_;
  }

  GateInput& getGateInputDriver()
  {
    return gate_input_;
  }

 private:

  Adc adc_;
  GateInput gate_input_;
  MultitapDelay* delay_;
  CalibrationData* calibration_data_;

  FAverage<32> average_[ADC_CHANNEL_LAST]{};
  FAverage<8> average_taptrig_{};
  FAverage<256> average_scale_{};
  FAverage<64> average_sync_ratio_{};
  float scale_hy_{}, scale_lp_{};
  OnePole fsr_filter_{};
  float previous_taptrig_{};
  bool taptrig_armed_{};
  uint32_t taptrig_counter_{};
  bool tapfsr_armed_{};
  bool clock_armed_{};
  float freq_lp_{};
  float amount_lp_{};
};

}

