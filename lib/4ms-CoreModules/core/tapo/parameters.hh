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

#pragma once

#include <array>

#include "stmlib/stmlib.h"

namespace TapoDelay {

const size_t kBlockSize = 64;
const uint8_t kMaxTaps = 32;

typedef struct { short l; short r; } ShortFrame;
typedef struct { float l; float r; } FloatFrame;

enum VelocityType {
  VELOCITY_AMP,
  VELOCITY_LP,
  VELOCITY_BP,
};

enum EditMode {
  EDIT_OFF = 0,
  EDIT_OVERDUB = 1,
  EDIT_NORMAL = 2,
};

enum PanningMode {
  PANNING_LEFT,
  PANNING_RANDOM,
  PANNING_ALTERNATE,
};

enum SequencerDirection {
  DIRECTION_FORWARD,
  DIRECTION_WALK,
  DIRECTION_RANDOM,
};

struct Parameters {
  float gain{};
  float scale{};
  float feedback{};
  float modulation_amount{};
  float modulation_frequency{};
  float morph{};
  float drywet{};
  float sync_ratio{};

  float velocity{};

  EditMode edit_mode{};
  VelocityType velocity_type{};
  SequencerDirection sequencer_direction{};

  // Settings
  float velocity_parameter{};
  PanningMode panning_mode{};
};

struct TapParameters {
  float time{};
  float velocity{};
  VelocityType velocity_type{};
  float panning{};
};

struct Slot {
  uint8_t size{};
  std::array<TapParameters,kMaxTaps> taps{};
};
}

