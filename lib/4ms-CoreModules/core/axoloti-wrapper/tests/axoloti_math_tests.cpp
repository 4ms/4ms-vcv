#include "axoloti-wrapper/axoloti_math.h"
#include "doctest.h"
#include <iostream>
#include <stdint.h>

constexpr float Q31_MAX_POSITIVE_VAL = static_cast<float>(0x7FFFFFFFUL);

float float_sin(int32_t x)
{
	int32_t q;
	// SINE2TINTERP(x, q);
	q = sin_q31(x);

	float f = q / Q31_MAX_POSITIVE_VAL;
	// std::cout << std::dec << "sin_q31(" << ((float)x / Q31_MAX_POSITIVE_VAL) << " == 0x" << std::hex << x << ") = " << f
	// 		  << " = 0x" << std::hex << q << std::endl;
	return f;
}

float sine2t_f(int32_t x)
{
	int32_t q = sine2t[x];
	float f = q / Q31_MAX_POSITIVE_VAL;
	// std::cout << std::dec << "sine2t[" << x << "] = " << f << " = 0x" << std::hex << q << std::endl;
	return f;
}

void SMMUL_CHECK(int32_t a, int32_t b, int32_t c)
{
	int32_t val = ___SMMUL(a, b);
	CHECK(val == c);
	// std::cout << std::dec << "___SMMUL(" << a << ", " << b << ") == " << val;
	// std::cout << " == ";
	// std::cout << std::hex << "___SMMUL(0x" << a << ", 0x" << b << ") == 0x" << val;
	// std::cout << std::endl;
}

TEST_CASE("Math tests")
{
	axoloti_math_init();
	float f;

	// for (int i = 0; i < 4097; i++)
	// 	sine2t_f(i);

	SUBCASE("___SMUMUL multiplies two signed 32-bit numbers and shifts down 32")
	{
		SUBCASE("Check all single-bit negative or positive values * any other such value")
		{
			// (1<<X * 1<<Y) >> 32 == 1 << (X+Y-32), where X+Y > 32
			// Should hold for - * -, + * +, - * +, and + * -
			for (int x = 0; x <= 30; x++) {
				for (int y = 0; y <= 30; y++) {
					if (x + y > 32) {
						SMMUL_CHECK(1 << x, 1 << y, 1 << (x + y - 32));
						SMMUL_CHECK(-(1 << x), 1 << y, -(1 << (x + y - 32)));
						SMMUL_CHECK(1 << x, -(1 << y), -(1 << (x + y - 32)));
						SMMUL_CHECK(-(1 << x), -(1 << y), 1 << (x + y - 32));
					} else {
						auto val = ___SMMUL(1 << x, 1 << y);
						bool is_1_or_0 = val == 1 || val == 0;
						CHECK(is_1_or_0);
					}
				}
			}
		}
		SUBCASE("0*0=0")
		{
			SMMUL_CHECK(0, 0, 0);
		}
		SUBCASE("Edge cases: INT32 MAX and MIN have correct answers when multiplied by each other")
		{
			//((2^31 - 1) * (2^31 - 1))  >> 32
			//= (2^31 * 2^31) >> 32 - (2 * 2^31) >> 32 + (1 >> 32)
			//= 2^(62-32) - 2^(32-32)
			//= 2^30 - 1
			SMMUL_CHECK(INT32_MAX, INT32_MAX, (1 << 30) - 1);

			//-2^31 * -2^31  >> 32 = -2^15 * -2^15 = 2^30
			SMMUL_CHECK(INT32_MIN, INT32_MIN, 1 << 30);

			SMMUL_CHECK(INT32_MAX, INT32_MIN, -(1 << 30));
			SMMUL_CHECK(INT32_MIN, INT32_MAX, -(1 << 30));
		}
	}

	//TODO: SMMLS, VSQRTF, CLZ, SSAT, USAT
	//TODO: more robust SMMLA
	SUBCASE("SMMLA")
	{
		int32_t x = 23'129'345;
		int32_t y = 971'671'890;
		int32_t z = 98765432;

		double fx = x;
		double fy = y;
		double fz = z;
		double ans = fx * fy / (double)(UINT32_MAX);
		ans += fz;
		int32_t int_ans = ans;
		
		CHECK(___SMMLA(x, y, z) == int_ans);
	}

	SUBCASE("sine2t table is 4096 elements and goes -1 to +1 in Q31 (signed + 31 fractional bits), or 0x80000000 to "
			"0x7FFFFFFF")
	{
		SUBCASE("Near zero at [0]")
		{
			CHECK(sine2t_f(0) == doctest::Approx(0.f));
		}
		SUBCASE("Peaks at +1 at [1023] or [1024]")
		{
			CHECK(sine2t_f(1023) == doctest::Approx(1.f));
			CHECK(sine2t_f(1024) == doctest::Approx(1.f));
		}
		SUBCASE("Near Zero at [2048]")
		{
			CHECK(sine2t_f(2048) == doctest::Approx(0.f));
		}
		SUBCASE("Near -1 at [3071] or [3072]")
		{
			CHECK(sine2t_f(3071) == doctest::Approx(-1.f));
			CHECK(sine2t_f(3072) == doctest::Approx(-1.f));
		}
		SUBCASE("Near Zero at [4095]")
		{
			CHECK(sine2t_f(4096) == doctest::Approx(0.f));
		}
	}

	SUBCASE("SINE2TINTERP (aka sin_q31) takes a Q12.19 value and interpolates sine2t table ")
	{
		SUBCASE("Zero at 0")
		{
			f = float_sin(0);
			CHECK(f == doctest::Approx(0.f));
		}
		SUBCASE("Peak at 0x40000000 == 1<<30, which is signed +0.5 or unsigned 0.25")
		{
			CHECK(float_sin(0x40000000) == doctest::Approx(1.0f));
			CHECK(float_sin(1 << 30) == doctest::Approx(1.0f));
		}
		SUBCASE("Zero at 0x80000000 == 1<<31, which is signed -1 and unsigned 0.5")
		{
			CHECK(float_sin(0x80000000) == doctest::Approx(0.0f));
			CHECK(float_sin(1 << 31) == doctest::Approx(0.0f));
		}
		SUBCASE("Zero at 0x7FFFFFFF, which is signed +1 and unsigned 0.5")
		{
			CHECK(float_sin(0x7FFFFFFF) == doctest::Approx(0.0f));
		}
		SUBCASE("Negative peak at 0xC0000000, which is signed -0.5 and unsigned 0.75")
		{
			CHECK(float_sin(0xC0000000) == doctest::Approx(-1.0f));
		}
	}

	SUBCASE("pitcht is a table of 256 elements and goes from nearly-0 to +1.0 in Q31")
	{
		//+0.00000845 .. +1.0 in Q31
		// for (auto p : pitcht)
		// 	std::cout << p << ", ";

		//TODO test this more!
		CHECK(mtof48k_ext_q31(0x2666668) == 0x554ab60);
	}
}
