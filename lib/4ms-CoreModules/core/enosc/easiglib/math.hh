#pragma once

// #include "../data.hh"
#include "numtypes.hh"

namespace easiglib
{

namespace Exp2Table
{
static constexpr int exp2_size = 1024;
static constexpr float exp2_increment = 1.000677130693066; // 2 ^ (1/exp2_size)

static consteval std::array<uint32_t, exp2_size> make_exp_table() {
	std::array<uint32_t, exp2_size> table;

	float x = 1.0f;
	for (auto &el : table) {
		el = static_cast<uint32_t>(x * (1 << 23));
		x *= exp2_increment;
	}

	return table;
}
} //namespace Exp2Table

struct Math {
	static constexpr f pi = 3.14159265358979323846264338327950288_f;

	// [0..1] --> [0..1], f(0)=0, f(1)=1, f'(0)=0, f'(1)=0
	static constexpr f fast_raised_cosine(f x) {
		return x * x * (3_f - x * 2_f);
	}

	// [0..1] --> [-1..1]
	static constexpr f faster_sine(f x) {
		x = (x * 2_f) - 1_f;
		return 4_f * (x - x * x.abs());
	}

	// WARNING untested:
	// [-1; +1] --> [-1; +1]
	static constexpr s1_15 faster_sine(u0_32 x) {
		s1_31 y = x.to_signed_scale();
		s1_15 z = s1_15::narrow(y * 2);
		z -= s1_15::narrow(z * z.abs());
		z *= 2;
		z = z.add_sat(z);
		return z;
	}

	// [0..1] --> [-1..1]
	static constexpr f fast_sine(f x) {
		f y = faster_sine(x);
		y = 0.225_f * (y * y.abs() - y) + y;
		return y;
	}

	static constexpr f fast_tanh(f x) {
		return x * (27_f + x * x) / (27_f + 9_f * x * x);
	}

	static f fast_exp2(f x) {
		static_assert(is_power_of_2(Exp2Table::exp2_size), "");
		constexpr int const BITS = Log2<Exp2Table::exp2_size>::val;

		typedef union {
			f f_repr;
			u32 i_repr;
			struct {
				uint32_t mantisa : 23;
				uint32_t exponent : 8;
				uint32_t sign : 1;
			} parts;
		} float_cast;

		constexpr f const fact = f(1 << 23);
		u32 i = u32(((x + 127_f) * fact));
		float_cast u = {.i_repr = i};

		u.parts.mantisa = exp2_table[u.parts.mantisa >> (23 - BITS)];
		return u.f_repr;
	}

	static constexpr f softclip1(f x) {
		x *= 0.6666_f;
		return (x * (3_f - x * x)) * 0.5_f;
	}

	static constexpr f softclip2(f x) {
		x *= 0.536_f;
		f s = x * x;
		return x * (1.875_f + s * (-1.25_f + 0.375_f * s));
	}

private:
	static constexpr std::array<uint32_t, Exp2Table::exp2_size> exp2_table = Exp2Table::make_exp_table();
};

} // namespace easiglib
