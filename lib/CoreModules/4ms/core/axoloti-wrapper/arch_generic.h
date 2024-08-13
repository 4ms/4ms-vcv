#include <cmath>
#include <cstdint>

//Some of this taken from cmsis arm_math.h:

static inline int32_t ___SMMUL(int32_t op1, int32_t op2)
{
	//mult_32x32_keep32(a, x, y)
	return (int32_t)(((int64_t)op1 * op2) >> 32);
}

static inline int32_t ___SMMLA(int32_t op1, int32_t op2, int32_t op3)
{
	//multAcc_32x32_keep32(a, x, y)
	return op3 + (int32_t)(((int64_t)op1 * op2) >> 32);
}

static inline int32_t ___SMMLS(int32_t op1, int32_t op2, int32_t op3)
{
	//multSub_32x32_keep32(a, x, y)
	return op3 - (int32_t)(((int64_t)op1 * op2) >> 32);
}

static inline float _VSQRTF(float op1)
{
	return sqrtf(op1);
}

static inline uint8_t __CLZ(uint32_t data)
{
	if (data == 0U) {
		return 32U;
	}

	uint32_t count = 0U;
	uint32_t mask = 0x80000000U;

	while ((data & mask) == 0U) {
		count += 1U;
		mask = mask >> 1U;
	}
	return count;
}

static inline int32_t __SSAT(int32_t val, uint32_t sat)
{
	if ((sat >= 1U) && (sat <= 32U)) {
		const int32_t max = (int32_t)((1U << (sat - 1U)) - 1U);
		const int32_t min = -1 - max;
		if (val > max) {
			return max;
		} else if (val < min) {
			return min;
		}
	}
	return val;
}

static inline uint32_t __USAT(int32_t val, uint32_t sat)
{
	if (sat <= 31U) {
		const uint32_t max = ((1U << sat) - 1U);
		if (val > (int32_t)max) {
			return max;
		} else if (val < 0) {
			return 0U;
		}
	}
	return (uint32_t)val;
}

static inline float q27_to_float(int32_t op1)
{
	float f_op1 = static_cast<float>(op1);
	float shift = static_cast<float>(1UL << 27UL);
	return f_op1 / shift;
}

static inline int32_t float_to_q27(float fop1)
{
	return fop1 * (1UL << 27UL);
}

static inline int32_t rand_s32(void)
{
	// return reinterpret_cast<int32_t>(rand());

	// This function differs from the standard C rand() definition, standard C
	// rand() only returns positive numbers, while rand_s32() returns the full
	// signed 32 bit range.
	static uint32_t randSeed = 22222;
	return randSeed = (randSeed * 196314165) + 1234567; // + RNG->DR;
}
