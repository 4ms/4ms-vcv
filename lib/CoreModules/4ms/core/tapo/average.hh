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
// Moving average filter

#pragma once

#include <algorithm>

namespace TapoDelay {

template<int SIZE>
class Average {
 public:

  void Init(int32_t x) {
    last = x * (SIZE-1);
    cursor = 0;
    std::fill(history, history+SIZE, x);
  }

  void Init() { Init(0); }

  void Process(int32_t x) {
    int32_t input = x;
    history[cursor++ % SIZE] = input;
    last += input - history[cursor % SIZE];
  }

  int32_t value() {
    return last / (SIZE-1);
  }

 private:
  int32_t last;
  int32_t history[SIZE];
  size_t cursor;
};

const int32_t kResolution = 65536;

template<int SIZE>
class FAverage {
public:

  void Init() { a.Init(); }

  void Process(float x) {
    a.Process(static_cast<int32_t>(x * kResolution));
  }

  float value() {
    return static_cast<float>(a.value()) / kResolution;
  }
private:
  Average<SIZE> a;
};

}

