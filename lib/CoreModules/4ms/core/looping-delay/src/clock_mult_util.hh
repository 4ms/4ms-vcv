#pragma once
#include "controls.hh"
#include "exp_1voct_lut.hh"
#include <cstdint>

namespace LDKit
{
struct ClockMultUtil {
	static constexpr float DivKnobValue[17] = {
		1.f, 1.5f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f};
	static constexpr int16_t DivKnobDetents[18] = {
		0, 40, 176, 471, 780, 1076, 1368, 1664, 1925, 2179, 2448, 2714, 2991, 3276, 3586, 3879, 4046, 4095};

	static constexpr float calc_quantized(int16_t adc_val) {
		bool divide = false;
		float val = DivKnobValue[16];

		if (adc_val < 0) {
			adc_val = -adc_val;
			divide = true;
		}
		val = DivKnobValue[_find_range_idx(adc_val)];
		return divide ? (1.f / val) : val;
	}

	static constexpr float calc_unquant(int16_t adc_val) {
		bool divide = false;
		if (adc_val < 0) {
			adc_val = -adc_val;
			divide = true;
		}

		float val;
		unsigned range_i = _find_range_idx(adc_val);
		if (range_i == 0)
			val = DivKnobValue[0];
		else {
			float floor = DivKnobValue[range_i - 1];
			float ceil = DivKnobValue[range_i];
			val = MathTools::map_value(adc_val, DivKnobDetents[range_i], DivKnobDetents[range_i + 1], floor, ceil);
		}
		return divide ? (1.f / val) : val;
	}

	static constexpr float calc_voct(int16_t adc_val, float tracking_comp) {
		float comp_cv = (float)adc_val * tracking_comp;
		return 1.f / exp_1voct[(unsigned)comp_cv];
	}

	static constexpr unsigned _find_range_idx(uint16_t adc_val) {
		static_assert(DivKnobDetents[0] == 0, "");
		unsigned i;
		for (i = 1; i < 17; i++) {
			if (adc_val <= DivKnobDetents[i])
				break;
		}
		return i - 1;
	}
};

static_assert(ClockMultUtil::calc_quantized(0) == 1.0f, "");
static_assert(ClockMultUtil::calc_quantized(39) == 1.0f, "");
static_assert(ClockMultUtil::calc_quantized(40) == 1.0f, "");
static_assert(ClockMultUtil::calc_quantized(41) == 1.5f, "");
static_assert(ClockMultUtil::calc_quantized(4045) == 15.f, "");
static_assert(ClockMultUtil::calc_quantized(4046) == 15.f, "");
static_assert(ClockMultUtil::calc_quantized(4047) == 16.f, "");
static_assert(ClockMultUtil::calc_quantized(4095) == 16.f, "");

static_assert(ClockMultUtil::_find_range_idx(39) == 0, "");
static_assert(ClockMultUtil::_find_range_idx(40) == 0, "");
static_assert(ClockMultUtil::_find_range_idx(41) == 1, "");

static_assert(ClockMultUtil::calc_unquant(39) == 1.0f, "");
static_assert(ClockMultUtil::calc_unquant(40) == 1.000f, "");
static_assert(ClockMultUtil::calc_unquant(41) >= 1.003f, "");
static_assert(ClockMultUtil::calc_unquant(41) <= 1.004f, "");
static_assert(ClockMultUtil::calc_unquant(42) >= 1.007f, "");
static_assert(ClockMultUtil::calc_unquant(42) <= 1.008f, "");
static_assert(ClockMultUtil::calc_unquant(780) == 3.0f, "");
static_assert(ClockMultUtil::calc_unquant(928) == 3.5f, "");

static_assert(ClockMultUtil::calc_unquant(4046) == 15.000f, "");

static_assert(ClockMultUtil::calc_unquant(4047) >= 15.020f, "");
static_assert(ClockMultUtil::calc_unquant(4047) <= 15.021f, "");

static_assert(ClockMultUtil::calc_unquant(4048) >= 15.040f, "");
static_assert(ClockMultUtil::calc_unquant(4048) <= 15.041f, "");

static_assert(ClockMultUtil::calc_unquant(4094) >= 15.979f, "");
static_assert(ClockMultUtil::calc_unquant(4094) <= 15.980f, "");

static_assert(ClockMultUtil::calc_unquant(4095) == 16.f, "");
} // namespace LDKit
