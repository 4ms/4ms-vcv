#pragma once

#include "../helpers/mapping.h"
#include "gcem/include/gcem.hpp"

namespace MetaModule
{

struct VoltageToFreqTableRange {
	static constexpr float min = -0.2f;
	static constexpr float max = 0.5f;
};

static constinit auto VoltageToFrequencyTable =
	Mapping::LookupTable_t<50>::generate<VoltageToFreqTableRange>([](auto voltage) {
		// voltage offset to not calculate with negative values during fitting
		// once the table is created, we can lookup negative voltages values just fine
		constexpr double VoltageOffset = 0.1;

		// two points in the V->f curve
		// taken from voltages from the model at specific switch positions and expected frequencies from the manual
		constexpr double V_1 = 0.31527 + VoltageOffset;
		constexpr double f_1 = 1.0 / (5 * 60) / 2;
		constexpr double V_2 = -0.055331 + VoltageOffset;
		constexpr double f_2 = 800 * 2;

		// helper constant to limit ranges of intermediate values and improve precision
		constexpr double ArgScalingFactor = 100.0;

		// This does not work in clang because math functions are not constexpr
		// It doesn't work with gcem either because of numeric problems
		// constexpr double arg = std::log2(f_1 / f_2) / (V_1 - V_2);
		// constexpr double b = std::pow(2.0f, arg / ArgScalingFactor);
		// constexpr double a = f_1 / std::pow(std::pow(2.0, arg), V_1);

		// So we just hardcode the factors that GCC produces for now
		constexpr double a = 8.4172569220933146e+3;
		constexpr double b = 6.8957132479261685e-1;

		auto coreFunc = [](double voltage) -> float {
			return float(gcem::pow(b, voltage * ArgScalingFactor) * a);
		};

		// make sure the fitting is correct
		static_assert(gcem::abs(coreFunc(V_1) - f_1) / f_1 < 1e-2f);
		static_assert(gcem::abs(coreFunc(V_2) - f_2) / f_2 < 1e-2f);

		// interpolate
		auto frequency = coreFunc((double)voltage + VoltageOffset);

		return frequency;
	});

// Convert voltage to time without dealing with details of transistor core
static auto VoltageToTime = [](float voltage) -> float {
    auto frequency = VoltageToFrequencyTable.lookup(voltage);

    // limit to valid frequency range
    frequency = std::clamp(frequency, 1.0f / (60 * 3), 20e3f);

    // convert to period length
    auto time = 1.0f / frequency;

    return time;
};

}