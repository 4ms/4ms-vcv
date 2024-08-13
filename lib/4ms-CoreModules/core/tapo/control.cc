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
// Knob and CV Controls.

#include "control.hh"

#include <algorithm>
#include <cmath>

#include "stmlib/dsp/dsp.h"

using namespace std;

namespace TapoDelay {

const float kPotDeadZoneSize = 0.01f;
const float kScalePotNotchSize = 0.07f;
const float kScaleHysteresis = 0.03f;
const float kSyncRatios[16] = {
  1.0f/8.0f, 1.0f/7.0f, 1.0f/6.0f, 1.0f/5.0f, 1.0f/4.0f, 1.0f/3.0f, 1.0f/2.0f,
  1.0f, 1.0f,
  2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 8.0f, 16.0f };

void Control::Init(MultitapDelay* delay, CalibrationData* calibration_data) {
  delay_ = delay;
  calibration_data_ = calibration_data;
  adc_.Init();
  gate_input_.Init();
  for (size_t i = 0; i < ADC_CHANNEL_LAST; ++i) {
    average_[i].Init();
  }
  fsr_filter_.Init();
  fsr_filter_.set_f<FREQUENCY_FAST>(0.01f);
  average_scale_.Init();
  average_sync_ratio_.Init();
  scale_lp_ = 1.0f;
  scale_hy_ = 1.0f;
}

void Control::Calibrate() {
  const int kCalibrationCycles = 16;

  float offset_value[ADC_CHANNEL_LAST] = {0};

  for (int c=0; c<kCalibrationCycles; c++) {
    adc_.Convert();
    adc_.Wait();
    for(size_t i=0; i<ADC_CHANNEL_LAST; i++) {
      offset_value[i] += adc_.float_value(i);
    }
  }

  for(size_t i=ADC_SCALE_CV; i<=ADC_DRYWET_CV; i++) {
    calibration_data_->offset[i-ADC_SCALE_CV] = offset_value[i] / kCalibrationCycles;
  }
}

inline float CropDeadZone(float x) {
  x = x * (1.0f + 2.0f * kPotDeadZoneSize) - kPotDeadZoneSize;
  CONSTRAIN(x, 0.0f, 1.0f);
  return x;
}


void Control::Read(Parameters* parameters, bool sequencer_mode) {

  float scaled_values[ADC_CHANNEL_LAST];

  /* 1. Apply pot laws */
  float val;

  // gain
  val = adc_.float_value(ADC_GAIN_POT);
  val *= val;          // quadratic
  scaled_values[ADC_GAIN_POT] = val;

  // scale
  val = adc_.float_value(ADC_SCALE_POT);
  scaled_values[ADC_SCALE_POT] = val;

  // feedback
  val = adc_.float_value(ADC_FEEDBACK_POT);
  val *= val;
  scaled_values[ADC_FEEDBACK_POT] = val;

  // modulation
  val = adc_.float_value(ADC_MODULATION_POT);
  scaled_values[ADC_MODULATION_POT] = val;

  // drywet
  val = adc_.float_value(ADC_DRYWET_POT);
  scaled_values[ADC_DRYWET_POT] = val;

  // morph
  val = adc_.float_value(ADC_MORPH_POT);
  scaled_values[ADC_MORPH_POT] = val;

  // clock ratio
  val = adc_.float_value(ADC_SCALE_POT);
  float scaled_sync_ratio = val;

  /* 2. Offset and scale CVs */

  for (int i=ADC_SCALE_CV; i<ADC_CHANNEL_LAST; i++) {
    if (i < ADC_CLOCK_CV) {
      // bipolar CVs with calibrated zero
      scaled_values[i] = calibration_data_->offset[i-ADC_SCALE_CV] - adc_.float_value(i);
      scaled_values[i] *= 1.05f;

    } else {
      // unipolar CVs
      scaled_values[i] = adc_.float_value(i);
    }
  }

  /* 3. Filter pots and CVs */

  for (size_t i=0; i<ADC_CHANNEL_LAST; i++) {
    float value = scaled_values[i];
    average_[i].Process(value);
  }

  average_sync_ratio_.Process(scaled_sync_ratio);

  /* 4. Add CV and pot, constrain, and write to parameters */

  // gain
  val = average_[ADC_GAIN_POT].value();
  val = CropDeadZone(val);
  val *= 3.0f;
  parameters->gain = val;

  // scale
  val =
    average_[ADC_SCALE_POT].value() +
    average_[ADC_SCALE_CV].value();
  val = CropDeadZone(val);
  val *= val;
  val *= 4.0f;

  if (val - scale_hy_ > kScaleHysteresis) {
    scale_hy_ = val - kScaleHysteresis;
  } else if (val - scale_hy_ < -kScaleHysteresis) {
    scale_hy_ = val + kScaleHysteresis;
  }
  val = scale_hy_;

  // flat zone at noon
  if (val < 1.0f - kScalePotNotchSize) {
    val += kScalePotNotchSize;
  } else if (val > 1.0f + kScalePotNotchSize) {
    val -= kScalePotNotchSize;
  } else {
    val = 1.0f;
  }

  average_scale_.Process(val);
  val = average_scale_.value();

  ONE_POLE(scale_lp_, val, 0.005f);
  parameters->scale = scale_lp_;

  // feedback
  val =
    average_[ADC_FEEDBACK_POT].value() +
    average_[ADC_FEEDBACK_CV].value();
  val = CropDeadZone(val);
  val *= 1.4f;
  parameters->feedback = val;

  // modulation
  val =
    average_[ADC_MODULATION_POT].value() +
    average_[ADC_MODULATION_CV].value();
  val = CropDeadZone(val);
  float amount = val;
  amount *= amount * amount;
  // offset avoids null frequency (NaN samples)
  float freq = (1.0f - val) * 0.43f + 0.0000001f;
  freq *= freq * freq * freq;

  ONE_POLE(freq_lp_, freq, 0.03f);
  ONE_POLE(amount_lp_, amount, 0.007f);

  parameters->modulation_amount = amount_lp_;
  parameters->modulation_frequency = freq_lp_;

  // drywet
  val =
    average_[ADC_DRYWET_POT].value() +
    average_[ADC_DRYWET_CV].value();
  val = CropDeadZone(val);
  parameters->drywet = val;

  // morph
  val = average_[ADC_MORPH_POT].value();
  val = CropDeadZone(val);
  val = (val + 0.1f) / 1.1f;
  val = val * val * val * val;
  val *= 600000.0f;
  parameters->morph = val;

  // clock
  float clock = scaled_values[ADC_CLOCK_CV];
  if (clock > 0.2f && clock_armed_) {
    delay_->ClockTick();
    clock_armed_ = false;
  }

  if (clock < 0.1f) {
    clock_armed_ = true;
  }

  // clock ratio
  val =
    average_sync_ratio_.value() +
    average_[ADC_SCALE_CV].value();
  val = CropDeadZone(val);
  val = val * 15.0f + 0.5f;
  parameters->sync_ratio = kSyncRatios[static_cast<int>(val)];

  // tap & velocity
  bool tap = false;

  // from external source:
  average_taptrig_.Process(scaled_values[ADC_TAPTRIG_CV]);
  float taptrig = average_taptrig_.value();
  float taptrig_deriv = taptrig - previous_taptrig_;
  previous_taptrig_ = taptrig;

  if (taptrig_deriv < 0.001f &&
      taptrig_counter_ > 15 &&
      taptrig_armed_) {
    tap = true;
    parameters->velocity = scaled_values[ADC_VEL_CV];
    taptrig_armed_ = false;
    taptrig_counter_ = 0;
  }

  taptrig_counter_++;

  if (taptrig_deriv > 0.02f) {
    taptrig_armed_ = true;
  }

  // from FSR:
  float fsr_deriv = fsr_filter_.Process<FILTER_MODE_HIGH_PASS>(average_[ADC_FSR_CV].value());

  if (fsr_deriv > 0.01f && tapfsr_armed_) {
    tap = true;
    tapfsr_armed_ = false;
    parameters->velocity = sequencer_mode ?
      0.6f - scaled_values[ADC_FSR_CV] :
      scaled_values[ADC_FSR_CV];
    if (parameters->velocity < 0) parameters->velocity = 0;
  }

  if (fsr_deriv < 0.005f) {
    tapfsr_armed_ = true;
  }

  if (tap) {
    if (sequencer_mode) {
      float val = parameters->velocity;
      val *= val;
      val *= 300000.0f;
      delay_->sequencer_step(val + parameters->morph);
    } else {
      delay_->AddTap(parameters);
    }
  }

  /////////////

  // repeat
  if (gate_input_.rising_edge(GATE_INPUT_REPEAT) ||
      gate_input_.falling_edge(GATE_INPUT_REPEAT)) {
    delay_->set_repeat(!delay_->repeat());
  }

  ///////////

  gate_input_.Read();
  adc_.Convert();
}

}