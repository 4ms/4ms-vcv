#pragma once
#include "../easiglib/dsp.hh"

namespace EnOsc
{

enum SpiAdcInput { CV_PITCH, CV_ROOT, NUM_SPI_ADC_CHANNELS };

class SpiAdc : easiglib::Nocopy {
	easiglib::u0_16 values[NUM_SPI_ADC_CHANNELS];

public:
	SpiAdc() = default;

	void switch_channel() {
	}

	void set(int i, easiglib::u0_16 v) {
		static_assert(NUM_SPI_ADC_CHANNELS == 2);
		values[i & 1] = v; //cheap bounds-checking
	}

	easiglib::u0_16 get(int i) {
		static_assert(NUM_SPI_ADC_CHANNELS == 2);
		return values[i & 1]; //cheap bounds-checking
	}
};

} // namespace EnOsc
