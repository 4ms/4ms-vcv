#pragma once

#include "numtypes.hh"

namespace easiglib
{

template<int WIDTH>
struct Bitfield {
  using Base = typename Basetype<WIDTH, UNSIGNED>::T;

  explicit constexpr Bitfield(Base const val) : val_(val) {}

  template<sign SIGN, int INT, int FRAC>
  Bitfield(Fixed<SIGN, INT, FRAC> x) : val_(x.repr()) {}

  Base repr() { return val_; }

  bool is_set(int i) {
    return (val_ & (1 << i)) != 0;
  }

  Bitfield set(int i) {
    return Bitfield(val_ | (1U << i));
  }

  Bitfield reset(int i) {
    return Bitfield(val_ & ~(1U << i));
  }

  int set_bits() {
    int n=val_, count=0;
    while (n) {
      n &= (n-1);
      count++;
    }
    return count; 
  }

  Bitfield operator&(Bitfield that) { return Bitfield(val_ & that.val_); }
  Bitfield operator|(Bitfield that) { return Bitfield(val_ | that.val_); }
  Bitfield operator^(Bitfield that) { return Bitfield(val_ ^ that.val_); }

  void operator^=(Bitfield that) { *this = *this ^ that; }

private:
  Base val_ = 0;
};

}
