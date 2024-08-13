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
// Simple ring buffer with multiple taps

#pragma once

#include "stmlib/stmlib.h"
#include "stmlib/dsp/dsp.h"

#include <algorithm>

namespace TapoDelay {

class AudioBuffer
{
 public:

  void Init(short* buffer, uint32_t buffer_size) {
    buffer_ = buffer;
    buffer_size_ = buffer_size;
  }

  void Clear() {
    std::fill(buffer_, buffer_ + buffer_size_, 0);
  }

  uint32_t size() { return buffer_size_; }

  /* Write one value at cursor and increment it */
  inline void Write(short value) {
    buffer_[cursor_] = value;
    if (cursor_ == buffer_size_) {
      cursor_ = 0;
    } else {
      cursor_++;
    }
  }

  /* Write [size] values at cursor and increment it. */
  inline void Write(short *src, size_t size) {
    size_t written = size;
    if (cursor_ > buffer_size_ - size) {
      written = buffer_size_ - cursor_;
    }
    std::copy(src, src + written, buffer_ + cursor_);
    if (written < size) {
      cursor_ = size - written;
      std::copy(src + written, src + size, buffer_);
    } else {
      cursor_ += size;
    }
  }

  /* Reads the value from [pos] writes ago */
  inline short ReadShort(uint32_t pos) {
    uint32_t index;// = cursor_ - pos;
    if (cursor_ < pos) {
      index = buffer_size_ - pos + cursor_;
    } else {
      index = cursor_ - pos;
    }
    return buffer_[index];
  }

  /* Assumes that buffer_size_ is 2^n */
  inline float ReadLinear(float pos) {
    MAKE_INTEGRAL_FRACTIONAL(pos);
    int32_t x = cursor_ - pos_integral;
    float a = buffer_[x & (buffer_size_-1)];
    float b = buffer_[(x - 1) & (buffer_size_-1)];
    return (a + (b - a) * pos_fractional) / 32768.0f;
  }

  /* Assumes that buffer_size_ is 2^n */
  inline float ReadHermite(float pos) {
    MAKE_INTEGRAL_FRACTIONAL(pos);
    int32_t x = cursor_ - pos_integral;
    float xm1 = buffer_[x & (buffer_size_-1)];
    float x0 = buffer_[(x - 1) & (buffer_size_-1)];
    float x1 = buffer_[(x - 2) & (buffer_size_-1)];
    float x2 = buffer_[(x - 3) & (buffer_size_-1)];
    float c = (x1 - xm1) * 0.5f;
    float v = x0 - x1;
    float w = c + v;
    float a = w + v + (x2 - x0) * 0.5f;
    float b_neg = w + a;
    float t = pos_fractional;
    return ((((a * t) - b_neg) * t + c) * t + x0) / 32768.0f;
  }

  /* Assumes that buffer_size_ is 2^n */
  inline float Read(float pos) {
    int32_t pos_integral = static_cast<uint32_t>(pos); \
    int32_t x = cursor_ - pos_integral;
    float a = buffer_[x & (buffer_size_-1)];
    return a / 32768.0f;
  }

  /* Reads the [size] values until [pos] writes ago.
   * assert (pos + size < buffer_size_) */
  inline void Read(short* dest, uint32_t pos, size_t size) {
    uint32_t index;
    size_t read = size;
    if (cursor_ < pos + size) {
      index = buffer_size_ - pos - size + cursor_;
      if (index > buffer_size_ - size) {
        read = buffer_size_ - index;
        std::copy(buffer_, buffer_ + size - read, dest + read);
      }
    } else {
      index = cursor_ - pos - size;
    }
    std::copy(buffer_ + index, buffer_ + index + read, dest);
  }

 private:

  short* buffer_;
  uint32_t cursor_;
  uint32_t buffer_size_;
};

}

