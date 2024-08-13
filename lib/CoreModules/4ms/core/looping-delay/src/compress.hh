#include <cstdint>

// TODO: When clang supports float template parameters, make `float Threshold` the second parameter
template<uint32_t MaxSampleVal, uint32_t ThresholdPercent>
constexpr inline int32_t compress(int32_t val) {
	constexpr float Threshold = ThresholdPercent / 100.f;
	constexpr float Max = (float)MaxSampleVal;
	constexpr float CompressionFactor = Max * Max * Threshold * (1.f - Threshold);
	constexpr int32_t ThresholdValue = Threshold * Max;

	float tv = CompressionFactor / (float)val;
	if (val > ThresholdValue)
		return int32_t(Max - tv);
	else if (val < -ThresholdValue)
		return int32_t(-Max - tv);
	else
		return val;
}

static inline constexpr int32_t thresh_top = 0x7FFFFF * 0.75f; // 0x5F'FFFF
static_assert(compress<0x7FFFFF, 75>(thresh_top) == thresh_top);

static_assert(compress<0x7FFFFF, 75>(thresh_top + 1) == thresh_top);

static_assert(compress<0x7FFFFF, 75>(thresh_top + 2) == thresh_top + 1);
static_assert(compress<0x7FFFFF, 75>(thresh_top + 3) == thresh_top + 1);
static_assert(compress<0x7FFFFF, 75>(thresh_top + 4) == thresh_top + 1);

static_assert(compress<0x7FFFFF, 75>(thresh_top + 5) == thresh_top + 2);
static_assert(compress<0x7FFFFF, 75>(thresh_top + 6) == thresh_top + 2);
static_assert(compress<0x7FFFFF, 75>(thresh_top + 7) == thresh_top + 2);

static_assert(compress<0x7FFFFF, 75>(thresh_top + 8) == thresh_top + 3);
static_assert(compress<0x7FFFFF, 75>(thresh_top + 9) == thresh_top + 3);
static_assert(compress<0x7FFFFF, 75>(thresh_top + 10) == thresh_top + 3);

static_assert(compress<0x7FFFFF, 75>(0x7FFFFF) == thresh_top + 0x80000);
static_assert(compress<0x7FFFFF, 75>(0x7FFFFF) == 0x67FFFF);

static inline constexpr int32_t thresh_bot = thresh_top * -1;
static_assert(compress<0x7FFFFF, 75>(thresh_bot) == thresh_bot);

static_assert(compress<0x7FFFFF, 75>(thresh_bot - 1) == thresh_bot);

static_assert(compress<0x7FFFFF, 75>(thresh_bot - 2) == thresh_bot - 1);
static_assert(compress<0x7FFFFF, 75>(thresh_bot - 3) == thresh_bot - 1);
static_assert(compress<0x7FFFFF, 75>(thresh_bot - 4) == thresh_bot - 1);

static_assert(compress<0x7FFFFF, 75>(thresh_bot - 5) == thresh_bot - 2);
static_assert(compress<0x7FFFFF, 75>(thresh_bot - 6) == thresh_bot - 2);
static_assert(compress<0x7FFFFF, 75>(thresh_bot - 7) == thresh_bot - 2);

static_assert(compress<0x7FFFFF, 75>(thresh_bot - 8) == thresh_bot - 3);
static_assert(compress<0x7FFFFF, 75>(thresh_bot - 9) == thresh_bot - 3);
static_assert(compress<0x7FFFFF, 75>(thresh_bot - 10) == thresh_bot - 3);

static_assert(compress<0x7FFFFF, 75>(0xFF800000) == thresh_bot - 0x80000);
static_assert(compress<0x7FFFFF, 75>(0xFF800000) == -6815743);
