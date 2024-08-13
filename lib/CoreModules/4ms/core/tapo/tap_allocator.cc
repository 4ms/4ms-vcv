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
// Tap allocator

#include "tap_allocator.hh"

namespace TapoDelay {

void TapAllocator::Init(Tap taps[kMaxTaps])
{
  taps_ = taps;
  fade_time_ = 10000.0f;
}

void TapAllocator::Load(Slot* slot)
{
  Clear();
  for (int i=0; i<slot->size; i++) {
    TapParameters p = slot->taps[i];
    Add(true, p.time, p.velocity, p.velocity_type, p.panning);
  }
}

void TapAllocator::Save(Slot* slot)
{
  slot->size = count_voices();

  for(int i=0; i<count_voices(); i++) {
    int index = (oldest_voice_ + i) % kMaxTaps;
    slot->taps[i].time = taps_[index].time();
    slot->taps[i].velocity = taps_[index].velocity();
    slot->taps[i].velocity_type = taps_[index].velocity_type();
    slot->taps[i].panning = taps_[index].panning();
  }
}

bool TapAllocator::Add(float time, float velocity,
                       VelocityType velocity_type,
                       float panning)
{
  return Add(false, time, velocity, velocity_type, panning);
}

bool TapAllocator::Add(bool loading, float time, float velocity,
                       VelocityType velocity_type,
                       float panning)
{
  if (writeable()) {

    taps_[next_voice_].fade_in(fade_time_);
    taps_[next_voice_].set_time(time);
    taps_[next_voice_].set_velocity(velocity, velocity_type);
    taps_[next_voice_].set_panning(panning);

    if (time > max_time_)
      max_time_ = time;

    next_voice_ = (next_voice_ + 1) % kMaxTaps;
    count_voices_++;

    return true;
  } else {
    // no taps left: queue and start fading out first voice
    if (!loading) {
      RemoveFirst();
    }
    TapParameters p = {time, velocity, velocity_type, panning};
    queue_.Overwrite(p);
    return false;
  }
}

void TapAllocator::RecomputeMaxTime()
{
  float max = 0.0f;
  for(int i=0; i<count_voices(); i++) {
    int index = (oldest_voice_ + i) % kMaxTaps;
    float time = taps_[index].time();
    if (time > max) max = time;
  }
  max_time_ = max;
}

void TapAllocator::RemoveFirst()
{
  if (!empty()) {
    taps_[oldest_voice_].fade_out(fade_time_ + 1.0f);
    oldest_voice_ = (oldest_voice_ + 1) % kMaxTaps;
    count_voices_--;
    RecomputeMaxTime();
  }
}

bool TapAllocator::RemoveLast()
{
  if (!empty()) {
    next_voice_--;
    if (next_voice_ < 0) next_voice_ += kMaxTaps;
    taps_[next_voice_].fade_out(fade_time_ + 1.0f);
    count_voices_--;
    RecomputeMaxTime();
    return true;
  } else {
    return false;
  }
}

void TapAllocator::Poll()
{
  while (queue_.readable() && writeable()) {
    TapParameters p = queue_.Read();
    Add(p.time, p.velocity, p.velocity_type, p.panning);
  }
}

void TapAllocator::Clear()
{
  for(int i=0; i<count_voices(); i++) {
    int index = (oldest_voice_ + i) % kMaxTaps;
    taps_[index].fade_out(fade_time_ + 1.0f);
  }
  queue_.Flush();
  max_time_ = 0.0f;
  oldest_voice_ = next_voice_;
  count_voices_ = 0;
}

}