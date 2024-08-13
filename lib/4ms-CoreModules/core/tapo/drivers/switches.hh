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
// Driver for the front panel switches.

#pragma once

#include "buttons.hh"
#include "../stmlib/stmlib.h"

#include <algorithm>
#include <array>

using namespace std;

namespace TapoDelay {

const uint8_t kNumSwitches = 2;
const uint8_t kNumBitPerSwitch = 2;

enum SwitchNames {
  SWITCH_EDIT,
  SWITCH_VELO,
};


class Switches {
 public:
  void Init() {
    std::fill(switch_state_.begin(), switch_state_.end(), 0);
    std::fill(previous_switch_state_.begin(), previous_switch_state_.end(), 0);
  }

  void Read() {
  }

  void set(uint8_t channel, uint8_t val)
  {
    previous_switch_state_[channel] = switch_state_[channel];
    switch_state_[channel] = val;
  }

  inline uint8_t state(int i) { return switch_state_[i]; }
  inline uint8_t switched(int i) {
    return switch_state_[i] != previous_switch_state_[i];
  }

private:
  std::array<uint8_t,kNumSwitches> switch_state_{};
  std::array<uint8_t,kNumSwitches> previous_switch_state_{};
};
}


