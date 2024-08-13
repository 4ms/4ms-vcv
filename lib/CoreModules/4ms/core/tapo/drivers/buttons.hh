// Copyright 2016 Matthias Puech.
//
// Author: Matthias Puech <matthias.puech@gmail.com>
// Based on code by: Olivier Gillet <ol.gillet@gmail.com>
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
// Driver for the front panel buttons.

#pragma once

#include "../stmlib/stmlib.h"

#include <algorithm>

using namespace std;

namespace TapoDelay {

const uint8_t kNumButtons = 8;

enum ButtonNames {
  BUTTON_1,
  BUTTON_2,
  BUTTON_3,
  BUTTON_4,
  BUTTON_5,
  BUTTON_6,
  BUTTON_REPEAT,
  BUTTON_DELETE,
};

class Buttons {
 public:
  void Init() {

    fill(&button_state_[0], &button_state_[kNumButtons], 0xff);
  }

  void Debounce() {}

  void set(uint8_t channel, bool val)
  {
    button_state_[channel] = button_state_[channel] << 1 | val;
  }

  inline bool released(uint8_t index) const {
    return button_state_[index] == 0x7f;
  }
  
  inline bool just_pressed(uint8_t index) const {
    return button_state_[index] == 0x80;
  }

  inline bool pressed(uint8_t index) const {
    return button_state_[index] == 0x00;
  }


private:
  uint8_t button_state_[kNumButtons]{};
};

}

