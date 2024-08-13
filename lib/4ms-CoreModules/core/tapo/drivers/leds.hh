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
// Driver for the status LEDs.

#pragma once

// activate to enable prototype 2 compatibility mode
// #define PROTO2

#include "../stmlib/stmlib.h"
#include <array>

namespace TapoDelay {

const uint8_t kNumLeds = 26;

enum LedColor {
  COLOR_BLACK,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_YELLOW,
  COLOR_BLUE,
  COLOR_MAGENTA,
  COLOR_CYAN,
  COLOR_WHITE,
};

enum LedNames {
  LED_BUT1_R,
  LED_BUT1_G,
  LED_BUT1_B,
  LED_BUT2_R,
  LED_BUT2_G,
  LED_BUT2_B,
  LED_BUT3_R,
  LED_BUT3_G,
  LED_BUT3_B,
  LED_BUT4_R,
  LED_BUT4_G,
  LED_BUT4_B,
  LED_BUT5_R,
  LED_BUT5_G,
  LED_BUT5_B,
  LED_BUT6_R,
  LED_BUT6_G,
  LED_BUT6_B,
  LED_DELETE_R,
  LED_DELETE_G,
  LED_DELETE_B,
  LED_REPEAT_R,
  LED_REPEAT_G,
  LED_REPEAT_B,
  LED_TAP,
  OUT_VELNORM,                  // normalization for Vel (not an LED)
};

class Leds {
 public:
  void Init() {
    Clear();
  }

  void Write() {}

  void Clear() {
    for (int i=0; i<kNumLeds; i++) {
      set(i, false);
    }
  }

  bool get(uint8_t channel)
  {
    return values_[channel];
  }

  void set(uint8_t channel, bool value) {
    values_[channel] = value;
  }

  void set_rgb(uint8_t channel, uint8_t color) {
    for (int i=0; i<3; i++) {
      set(channel * 3 + i, (color >> i) & 1);
    }
  }

  void set_repeat(uint8_t color) {
    set(LED_REPEAT_R, (color >> 0) & 1);
    set(LED_REPEAT_G, (color >> 1) & 1);
    set(LED_REPEAT_B, (color >> 2) & 1);
  }

  void set_delete(uint8_t color) {
    set(LED_DELETE_R, (color >> 0) & 1);
    set(LED_DELETE_G, (color >> 1) & 1);
    set(LED_DELETE_B, (color >> 2) & 1);
  }

 private:
  std::array<bool,kNumLeds> values_;
};

}

