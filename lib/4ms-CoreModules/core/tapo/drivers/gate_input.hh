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
// Driver for the gate inputs.

#pragma once

#include "../stmlib/stmlib.h"
#include <array>

namespace TapoDelay {

enum GateNames {
  GATE_INPUT_REPEAT,
  GATE_INPUT_LAST
};

class GateInput {
 public:
  void Init() {
  }

  inline void set(uint8_t channel, bool newVal)
  {
    previous_values_[channel] = values_[channel];
    values_[channel] = newVal;
  }

  inline void Read() {}

	inline bool value(int8_t channel) const { return values_[channel]; }

	inline bool rising_edge(int8_t channel) const {
		return values_[channel] && !previous_values_[channel];
  }
	inline bool falling_edge(int8_t channel) const {
		return !values_[channel] && previous_values_[channel];
  }

 private:
	bool previous_values_[GATE_INPUT_LAST]{};
	bool values_[GATE_INPUT_LAST]{};
};

}

