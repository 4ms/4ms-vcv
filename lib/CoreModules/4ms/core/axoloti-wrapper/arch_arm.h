#pragma once
#define __PROGRAM_START
#include "arm_math.h"
// #include "cmsis_gcc.h"

#include <cstdint>

static inline int32_t ___SMMUL(int32_t op1, int32_t op2)
{
	int32_t result;

	asm volatile("smmul %0, %1, %2" : "=r"(result) : "r"(op1), "r"(op2));
	return (result);
}

static inline int32_t ___SMMLA(int32_t op1, int32_t op2, int32_t op3)
{
	int32_t result;

	asm volatile("smmla %0, %1, %2, %3" : "=r"(result) : "r"(op1), "r"(op2), "r"(op3));
	return (result);
}

static inline int32_t ___SMMLS(int32_t op1, int32_t op2, int32_t op3)
{
	int32_t result;

	asm volatile("smmls %0, %1, %2, %3" : "=r"(result) : "r"(op1), "r"(op2), "r"(op3));
	return (result);
}

static inline float _VSQRTF(float op1)
{
	float result;
	asm volatile("vsqrt.f32 %0, %1" : "=w"(result) : "w"(op1));
	return (result);
}

static inline float q27_to_float(int32_t op1)
{
	float fop1 = *(float *)(&op1);
	asm volatile("VCVT.F32.S32 %0, %0, 27" : "+w"(fop1));
	return (fop1);
}

static inline int32_t float_to_q27(float fop1)
{
	asm volatile("VCVT.S32.F32 %0, %0, 27" : "+w"(fop1));
	int32_t r = *(int32_t *)(&fop1);
	return (r);
}

static inline int32_t rand_s32(void)
{
	// This function differs from the standard C rand() definition, standard C
	// rand() only returns positive numbers, while rand_s32() returns the full
	// signed 32 bit range.
	// The hardware random generator can't provide new data as quick as desireable
	// but rather than waiting for a new true random number,
	// we multiply/add the seed with the latest hardware-generated number.
	static uint32_t randSeed = 22222;
	return randSeed = (randSeed * 196314165) + 1234567; // + RNG->DR;
}
