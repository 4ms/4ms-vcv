// Copyright 2012 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
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
// System time.

#ifndef STMLIB_SYSTEM_SYSTEM_CLOCK_H_
#define STMLIB_SYSTEM_SYSTEM_CLOCK_H_

#include "../stmlib.h"

namespace TapoDelay::stmlib {

class SystemClock {
 public:
  SystemClock() = default;

  inline void Init() { count_ = 0; }
  inline void Tick() { ++count_; }
  inline uint32_t milliseconds() const { return count_; }
  inline void Delay(uint32_t ms) {
    uint32_t target = milliseconds() + ms;
    while (milliseconds() <= target);
  }

 private:
  uint32_t count_ = 0;
};

}  // namespace stmlib

#endif  // STMLIB_SYSTEM_SYSTEM_CLOCK_H_
