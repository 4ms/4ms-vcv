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
// Fade signal in/out


#pragma once

#include "stmlib/dsp/dsp.h"

#include "parameters.hh"

namespace TapoDelay {

class Fader {
 public:
  void Init() {
    volume_ = 0.0f;
    volume_increment_ = 0.0f;
  }

  inline float volume() { return volume_; }
  inline bool active() { return volume_ > 0.0f || volume_increment_ > 0.0f; }

  inline void fade_in(float length) {
    volume_increment_ = 1.0f / length;
    if (volume_ < 0.0f) volume_ = 0.0f;
  }

  inline void fade_out(float length) {
    volume_increment_ = -1.0f / length;
    if (volume_ > 1.0f) volume_ = 1.0f;
  }

  inline void Prepare() {
    if (volume_ < 0.0f) {
      volume_ = 0.0f;
      volume_increment_ = 0.0f;
    } else if (volume_ > 1.0f) {
      volume_ = 1.0f;
      volume_increment_ = 0.0f;
    }
  }

  inline void Process(float &sample) {
    sample *= volume_;
    volume_ += volume_increment_;
  }

  inline void Process(short &sample) {
    sample *= volume_;
    volume_ += volume_increment_;
  }

 private:
  float volume_;
  float volume_increment_;
};

}

