#pragma once
#include "peg-common/envelope_calcs.h"
#include "util/math.hh"

namespace MetaModule::PEG {

class MiniPEGEnvelopeCalcs : public EnvelopeCalcsBase {

	enum ShapeRegions {
		RAMPUP_EXP2LIN,
		RAMPUP2SYM_LIN2EXP,
		SYM_EXP2LOG,
		SYM2RAMPDOWN_LOG2LIN,
		RAMPDOWN_EXP2LIN,

		NUM_SHAPE_REGIONS
	};

	static constexpr unsigned NUM_DIVMULTS = 19;
	static constexpr int8_t P_array[NUM_DIVMULTS] = {32, 16, 8, 7, 6, 5, 4, 3, 2, 1, -2, -3, -4, -5, -6, -7, -8, -12, -16};
	static constexpr int16_t midpt_array[NUM_DIVMULTS] = {68, 262, 509, 743, 973, 1202, 1427, 1657, 1882, 
														  2107, 2341, 2574, 2802, 3026, 3262, 3500, 3734, 3942, 4095};

public:
	int8_t get_clk_div_nominal(uint16_t adc_val) override;

	// shape: 0..4095 (adc value)
	// returns skew: 0..255
	// returns next_curve_rise and _fall: 0..255: expo/linear/log
	void calc_skew_and_curves(uint16_t skewadc, uint16_t shapeadc, uint8_t *skew, uint8_t *next_curve_rise, uint8_t *next_curve_fall) override;

	static constexpr std::array<unsigned, 6> region_starts = {0, 900, 1450, 2645, 3195, 4096};
	static constexpr auto region_sizes = MathTools::array_adj_diff(region_starts);
	//{900, 550,...}


};

}
