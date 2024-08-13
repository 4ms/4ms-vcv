#pragma once
#include "numtypes.hh"

namespace easiglib
{

struct Signal {

  static f crossfade(f x, f y, f phase) {
    return x + (y - x) * phase;
  }

  static f crossfade(f x, f y, u0_16 phase) {
    return crossfade(x, y, f(phase));
  }

  static f crossfade(f x, f y, u0_32 phase) {
    return crossfade(x, y, f(phase));
  }

  static constexpr s1_15 crossfade(s1_15 x, s1_15 y, u0_32 phase) {
    return x + s1_15::narrow((y - x) * u0_16::narrow(phase).to_signed());
  }

  static constexpr s1_15 crossfade(s1_15 x, s1_15 y, u0_16 phase) {
    return x + s1_15::narrow((y - x) * phase.to_signed());
  }

  static constexpr u0_8 crossfade(u0_8 x, u0_8 y, u0_8 phase) {
    return x + u0_8::narrow(((y.to_signed() - x.to_signed()) * phase.to_signed()).to_unsigned());
  }

  static constexpr s1_15 crossfade_with_diff(s1_15 a, s1_15 d, u0_32 fractional) {
    return a + s1_15::narrow(d * u0_16::narrow(fractional).to_signed());
  }

  static constexpr f crossfade_with_diff(f a, f d, f fractional) {
    return a + d * fractional;
  }

  static constexpr f crossfade_with_diff(f a, f d, u0_32 fractional) {
    return a + d * f(fractional);
  }

  // (p..1 -> 0..1)
  static f crop_down(f p, f x) {
    return ((x-p) / (1_f-p)).max(0_f);
  }

  // 0..(1-p) -> 0..1
  static f crop_up(f p, f x) {
    return (x / (1_f - p)).min(1_f);
  }

  // p..(1-p) -> 0..1
  static f crop(f p, f x) {
    return ((x - p) / (1_f - 2_f * p)).min(1_f).max(0_f);
  }
};

}
