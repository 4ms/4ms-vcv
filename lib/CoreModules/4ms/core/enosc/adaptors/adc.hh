#pragma once
#include "../easiglib/dsp.hh"

namespace EnOsc
{

enum AdcInput {
	POT_WARP,
	POT_DETUNE,
	POT_MOD,
	POT_ROOT,
	POT_SCALE,
	POT_PITCH,
	POT_SPREAD,
	POT_BALANCE,
	POT_TWIST,
	CV_SPREAD,
	CV_WARP,
	CV_TWIST,
	CV_BALANCE,
	CV_SCALE,
	CV_MOD,
	ADC_INPUT_MAX
};

using namespace easiglib; //for literal suffix

class Adc : Nocopy {
	u0_16 values[ADC_INPUT_MAX];

public:
	Adc() = default;
	void set(AdcInput i, u0_16 v) {
		if (i < ADC_INPUT_MAX && i >= 0)
			values[i] = v;
	}

	u0_16 get(AdcInput i) {
		if (i < ADC_INPUT_MAX && i >= 0)
			return values[i];

		return 0._u0_16;
	}
};

} // namespace EnOsc
