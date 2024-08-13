#include "numtypes.hh"

// STATIC TESTS:
namespace easiglib
{

static_assert(65536_u16 == 0_u16, "");
static_assert(f(10_u).repr() == 10.f, "");
static_assert(f(12495_u) == 12495_f, "");
static_assert((32_u).min(42_u) == 32_u, "");
static_assert((32_u).max(42_u) == 42_u, "");
static_assert((42_u).min(32_u) == 32_u, "");
static_assert((42_u).max(32_u) == 42_u, "");
static_assert(f(-0.5_s1_15) == -0.5_f, "");
static_assert(f(-0.125_s1_15 - 0.5_s1_15) == -0.625_f, "");

static_assert(min_val<s1_15>.pred() == max_val<s1_15>, "");
static_assert(max_val<s1_15>.succ() == min_val<s1_15>, "");
static_assert(min_val<u0_16>.pred() == max_val<u0_16>, "");
static_assert(max_val<u0_16>.succ() == min_val<u0_16>, "");

// static_assert(min_val<s1_31>.pred() == max_val<s1_31>, "");
// static_assert(max_val<s1_31>.succ() == min_val<s1_31>, "");
static_assert(min_val<u0_32>.pred() == max_val<u0_32>, "");
static_assert(max_val<u0_32>.succ() == min_val<u0_32>, "");

static_assert(-1.0_s1_15 == min_val<s1_15>, "");
static_assert((1.0_s1_15).pred() == max_val<s1_15>, "");

static_assert(f(-0.5_s1_15) == -0.5_f, "");
static_assert(1.0_s1_15 == -1.0_s1_15, "");
static_assert(f(-0.125_s1_15 - 0.5_s1_15) == -0.625_f, "");
static_assert(1.0_u0_16 == 0._u0_16, "");
static_assert(f(0.5_u0_16 + 0.125_u0_16) == 0.625_f, "");
static_assert(f(0.25_u0_16 - 0.125_u0_16) == 0.125_f, "");
static_assert(f(0.25_u0_16 - 0.5_u0_16) == 0.75_f, "");

// TODO: constexpr problem with this one:
// static_assert(-1.0_s1_31 == 1.0_s1_31, "");

static_assert(f(0.45_u0_32) == 0.45_f, "");
static_assert(0.5_u0_32 == 0.5_u0_32, "");
static_assert(f(0.345_u0_32) == 0.345_f, "");
static_assert(1.0_u0_32 == 0.0_u0_32, "");
static_assert(0.625_u0_32 + 0.125_u0_32 == 0.75_u0_32, "");
static_assert(f(-0.5_s1_31) == -0.5_f, "");
static_assert(f(-0.5_s1_31+0.75_s1_31) == 0.25_f, "");

static_assert(1024.0_u10_22 == 0.0_u10_22, "");
static_assert(742.625_u10_22 == 742.625_u10_22, "");
static_assert((-0.625_s1_15).floor() == -1._s1_15, "");
static_assert((742.625_u10_22).floor() == 742.0_u10_22, "");
static_assert((742.625_u10_22).integral() == 742, "");
static_assert((742.625_u10_22).frac() == 0.625_u10_22, "");
static_assert((742.625_u10_22).floor() + (742.625_u10_22).frac() == 742.625_u10_22, "");
static_assert((-0.25_s1_15).floor() + (-0.25_s1_15).frac() == -0.25_s1_15, "");

// TODO test division
#ifndef __arm__
// static_assert(42.5_u10_22 / 4.0_u10_22 == 10.625_u10_22, "");
#endif

static_assert((742.625_u10_22).min(700.0_u10_22) == 700.0_u10_22, "");
static_assert((742.625_u10_22).max(700.0_u10_22) == 742.625_u10_22, "");
static_assert((742.625_u10_22).clip(700.0_u10_22, 750.0_u10_22) == 742.625_u10_22, "");
static_assert((642.625_u10_22).clip(700.0_u10_22, 750.0_u10_22) == 700.0_u10_22, "");
static_assert((842.625_u10_22).clip(700.0_u10_22, 750.0_u10_22) == 750.0_u10_22, "");

static_assert((-242.625_s10_22).min(-200.0_s10_22) == -242.625_s10_22, "");
static_assert((-242.625_s10_22).max(-200.0_s10_22) == -200.0_s10_22, "");
static_assert((-242.625_s10_22).clip(-250.0_s10_22, -200.0_s10_22) == -242.625_s10_22, "");
static_assert((-142.625_s10_22).clip(-250.0_s10_22, -200.0_s10_22) == -200.0_s10_22, "");
static_assert((-342.625_s10_22).clip(-250.0_s10_22, -200.0_s10_22) == -250.0_s10_22, "");

#ifndef __arm__
static_assert((0.9_u10_22).clip() == 0.9_u10_22, "");
static_assert((1.2_u10_22).clip() == (1.0_u10_22).pred(), "");
static_assert((-0.9_s10_22).clip() == -0.9_s10_22, "");
static_assert((-1.2_s10_22).clip() == -1.0_s10_22, "");
#endif

// pure widening
static_assert(u32(42_u) == u32(42_u));
static_assert(f(u32(42_u)) == 42_f, "");
static_assert(f(s32(42_s)) == 42_f, "");
static_assert(f(s32(-42_s)) == -42_f, "");
// widening with shift
static_assert(u0_32(0.5_u0_16) == 0.5_u0_32, "");
static_assert(s10_22(0.5_s1_15) == 0.5_s10_22, "");
static_assert(s10_22(-0.5_s1_15) == -0.5_s10_22, "");
// narrowing (fractional part)
static_assert(s1_15::narrow(0.50001_s1_31) == 0.5_s1_15, "");
static_assert(s1_15::narrow(-0.49999_s1_31) == -0.5_s1_15, "");
// saturating
#ifndef __arm__
static_assert((0.25_s10_22).to_sat<1, 31>() == 0.25_s1_31, "");
static_assert((-0.25_s10_22).to_sat<1, 31>() == -0.25_s1_31, "");
static_assert((0.75_s10_22).to_sat<1, 31>() == 0.75_s1_31, "");
static_assert((-0.75_s10_22).to_sat<1, 31>() == -0.75_s1_31, "");
// TODO
// static_assert((1.25_s10_22).to_sat<1, 31>() == max_val<s1_31>, "");
static_assert((-1.25_s10_22).to_sat<1, 31>() == min_val<s1_31>, "");
static_assert((0.25_u10_22).to_sat<0, 32>() == 0.25_u0_32, "");
static_assert((0.75_u10_22).to_sat<0, 32>() == 0.75_u0_32, "");
// TODO
// static_assert((1.25_u10_22).to_sat<0, 32>() == max_val<s0_32>, "");
#endif

#ifndef __arm__
static_assert((0.25_s1_15).add_sat(0.5_s1_15) == 0.75_s1_15, "");
static_assert((0.75_s1_15).add_sat(0.5_s1_15) == (1.0_s1_15).pred(), "");
static_assert((-0.75_s1_15).add_sat(-0.5_s1_15) == -1.0_s1_15, "");
static_assert((-0.75_s1_15).add_sat(-0.5_s1_15) == -1.0_s1_15, "");

static_assert((0.25_s1_15).sub_sat(0.5_s1_15) == -0.25_s1_15, "");
static_assert((0.75_s1_15).sub_sat(0.5_s1_15) == 0.25_s1_15, "");
static_assert((-0.75_s1_15).sub_sat(-0.5_s1_15) == -0.25_s1_15, "");
static_assert((0.75_s1_15).sub_sat(-0.5_s1_15) == (1.0_s1_15).pred(), "");

static_assert((0.25_u0_16).add_sat(0.5_u0_16) == 0.75_u0_16, "");
static_assert((0.75_u0_16).add_sat(0.5_u0_16) == (1.0_u0_16).pred(), "");
static_assert((0.75_u0_16).sub_sat(0.5_u0_16) == 0.25_u0_16, "");

static_assert((0.25_s1_31).add_sat(0.5_s1_31) == 0.75_s1_31, "");
#ifndef __MINGW32__
static_assert((0.75_s1_31).add_sat(0.5_s1_31) == (1.0_s1_31).pred(), "");
#endif
static_assert((-0.75_s1_31).add_sat(-0.5_s1_31) == 1.0_s1_31, "");
static_assert((-0.75_s1_31).add_sat(-0.5_s1_31) == 1.0_s1_31, "");

static_assert((0.25_s1_31).sub_sat(0.5_s1_31) == -0.25_s1_31, "");
static_assert((0.75_s1_31).sub_sat(0.5_s1_31) == 0.25_s1_31, "");
static_assert((-0.75_s1_31).sub_sat(-0.5_s1_31) == -0.25_s1_31, "");
#ifndef __MINGW32__
static_assert((0.75_s1_31).sub_sat(-0.5_s1_31) == (1.0_s1_31).pred(), "");
#endif 

static_assert((0.25_u0_32).add_sat(0.5_u0_32) == 0.75_u0_32, "");
static_assert((0.75_u0_32).add_sat(0.5_u0_32) == (1.0_u0_32).pred(), "");
static_assert((0.75_u0_32).sub_sat(0.5_u0_32) == 0.25_u0_32, "");
#endif

static_assert((0.5_u0_32).div2<1>() == 0.25_u0_32, "");
static_assert((0.5_u0_32).div2<4>() == 0.031250_u0_32, "");
static_assert((-0.5_s1_31).div2<4>() == -0.031250_s1_31, "");

// Signed/Unsigned

static_assert((0.75_u0_16).to_signed() == 0.75_s1_15, "");
static_assert((0.999_u0_16).to_signed() == 0.999_s1_15, "");
static_assert(s1_15::narrow(0.75_s1_31).to_unsigned() == 0.75_u0_16, "");
// static_assert(s1_15::narrow(-0.75_s1_31).to_unsigned() == 0.25_u0_16, "");
// static_assert(s1_15::narrow(-0.1_s1_31).to_unsigned() == 0.9_u0_16, "");

static_assert((0.75_u0_16).to_signed().to_unsigned() == 0.75_u0_16, "");
static_assert((0.33_u0_16).to_signed().to_unsigned() == 0.33_u0_16, "");
static_assert((0.0_u0_16).to_signed().to_unsigned() == 0.0_u0_16, "");
// TODO: is this normal?
static_assert((max_val<u0_16>).to_signed().to_unsigned() == max_val<u0_16>.pred(), "");

static_assert((0.75_s1_15).to_unsigned().to_signed() == 0.75_s1_15, "");
static_assert((0.33_s1_15).to_unsigned().to_signed() == 0.33_s1_15, "");
static_assert((0._s1_15).to_unsigned().to_signed() == 0.0_s1_15, "");
static_assert((max_val<s1_15>).to_unsigned().to_signed() == max_val<s1_15>, "");

// Multiplication

static_assert(0.5_s1_15 * 0.5_s1_15 == 0.25_s1_31);
static_assert(0.25_s1_15 * 0.75_s1_15 == 0.1875_s1_31);
static_assert(0._s1_15 * -0.75_s1_15 == 0._s1_31);
static_assert(-0.5_s1_15 * 0.5_s1_15 == -0.25_s1_31);
static_assert(-0.25_s1_15 * -0.5_s1_15 == 0.125_s1_31);

static_assert(0.25_u0_16 * 0.5_u0_16 == 0.125_u0_32);
static_assert(0._u0_16 * 0.5_u0_16 == 0._u0_32);
static_assert(1.0_u0_16 * 0.5_u0_16 == 0.0_u0_32);

// Division
// static_assert(42_u16 / 2_u16 == 21_u32);

// DYNAMIC TESTS

#if defined(__arm__) && defined(TEST)

struct Numtypes_Tests {
  Numtypes_Tests() {
    assert_param((0.9_u10_22).clip() == 0.9_u10_22);
    assert_param((1.2_u10_22).clip() == (1.0_u10_22).pred());
    assert_param((-0.9_s10_22).clip() == -0.9_s10_22);
    assert_param((-1.2_s10_22).clip() == -1.0_s10_22);
    assert_param(((0.25_s10_22).to_sat<1, 31>() == 0.25_s1_31));
    assert_param(((-0.25_s10_22).to_sat<1, 31>() == -0.25_s1_31));
    assert_param(((0.75_s10_22).to_sat<1, 31>() == 0.75_s1_31));
    assert_param(((-0.75_s10_22).to_sat<1, 31>() == -0.75_s1_31));
    // TODO
    // assert_param(((1.25_s10_22).to_sat<1, 31>() == max_val<s1_31>));
    // TODO
    // assert_param(((-1.25_s10_22).to_sat<1, 31>() == max_val<s1_31>));
    assert_param(((0.25_u10_22).to_sat<0, 32>() == 0.25_u0_32));
    assert_param(((0.75_u10_22).to_sat<0, 32>() == 0.75_u0_32));
    // TODO
    // assert_param(((1.25_u10_22).to_sat<0, 32>() == max_val<u0_32>));
    assert_param(((0.25_s1_15).add_sat(0.5_s1_15) == 0.75_s1_15));
    assert_param(((0.75_s1_15).add_sat(0.5_s1_15) == (1.0_s1_15).pred()));
    assert_param(((-0.75_s1_15).add_sat(-0.5_s1_15) == -1.0_s1_15));
    assert_param(((-0.75_s1_15).add_sat(-0.5_s1_15) == -1.0_s1_15));
    assert_param(((0.25_s1_15).sub_sat(0.5_s1_15) == -0.25_s1_15));
    assert_param(((0.75_s1_15).sub_sat(0.5_s1_15) == 0.25_s1_15));
    assert_param(((-0.75_s1_15).sub_sat(-0.5_s1_15) == -0.25_s1_15));
    assert_param(((0.75_s1_15).sub_sat(-0.5_s1_15) == (1.0_s1_15).pred()));
    assert_param(((0.25_u0_16).add_sat(0.5_u0_16) == 0.75_u0_16));
    assert_param(((0.75_u0_16).add_sat(0.5_u0_16) == (1.0_u0_16).pred()));
    assert_param(((0.75_u0_16).sub_sat(0.5_u0_16) == 0.25_u0_16));
    assert_param(((0.25_s1_31).add_sat(0.5_s1_31) == 0.75_s1_31));
    assert_param(((0.75_s1_31).add_sat(0.5_s1_31) == (1.0_s1_31).pred()));
    assert_param(((-0.75_s1_31).add_sat(-0.5_s1_31) == 1.0_s1_31));
    assert_param(((-0.75_s1_31).add_sat(-0.5_s1_31) == 1.0_s1_31));
    assert_param(((0.25_s1_31).sub_sat(0.5_s1_31) == -0.25_s1_31));
    assert_param(((0.75_s1_31).sub_sat(0.5_s1_31) == 0.25_s1_31));
    assert_param(((-0.75_s1_31).sub_sat(-0.5_s1_31) == -0.25_s1_31));
    assert_param(((0.75_s1_31).sub_sat(-0.5_s1_31) == (1.0_s1_31).pred()));
    assert_param(((25_f).sqrt() == 5_f));
  }
} numtypes_tests;

#endif

}
