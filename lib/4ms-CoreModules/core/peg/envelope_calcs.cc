#include "envelope_calcs.hh"
#include <algorithm>

namespace MetaModule::PEG
{

int8_t PEGEnvelopeCalcs::get_clk_div_nominal(uint16_t adc_val) {
	for (uint8_t i = 0; i < NUM_DIVMULTS; i++) {
		if (adc_val <= midpt_array[i])
			return (P_array[i]);
	}
	return (P_array[NUM_DIVMULTS - 1]);
}

void PEGEnvelopeCalcs::calc_skew_and_curves(uint16_t skewadc, uint16_t shapeadc, uint8_t *skew, uint8_t *next_curve_rise, uint8_t *next_curve_fall) {
	//TODO
	*skew = std::clamp(skewadc / 16, 0, 255); //0..4095 => 0..255

	//TODO: select curve from shapeadc
	auto curve_adc = std::clamp(shapeadc / 16, 0, 255); //0..4095 => 0..255

	if (curve_adc < 15) {
		*next_curve_rise = PureCurves::EXPO;
		*next_curve_fall = PureCurves::LOG;
	} else if (curve_adc <= 30) {
		*next_curve_rise = PureCurves::EXPO;
		*next_curve_fall = 191; // was LIN50
	} else if (curve_adc <= 45) {
		*next_curve_rise = PureCurves::EXPO;
		*next_curve_fall = PureCurves::LIN;
	} else if (curve_adc <= 60) {
		*next_curve_rise = PureCurves::EXPO;
		*next_curve_fall = 64; //was EXP50
	} else if (curve_adc <= 75) {
		*next_curve_rise = PureCurves::EXPO;
		*next_curve_fall = PureCurves::EXPO;
	} else if (curve_adc <= 90) {
		*next_curve_rise = 32; //was EXP75
		*next_curve_fall = 32; //was EXP75
	} else if (curve_adc <= 102) {
		*next_curve_rise = 64; //was EXP50
		*next_curve_fall = 64; //was EXP50
	} else if (curve_adc <= 120) {
		*next_curve_rise = 96; //was EXP25
		*next_curve_fall = 96; //was EXP25
	} else if (curve_adc <= 135) {
		*next_curve_rise = PureCurves::LIN;
		*next_curve_fall = PureCurves::LIN;
	} else if (curve_adc <= 150) {
		*next_curve_rise = 160; //was LIN75
		*next_curve_fall = 160; //was LIN75
	} else if (curve_adc <= 165) {
		*next_curve_rise = 191; // was LIN50
		*next_curve_fall = 191; // was LIN50
	} else if (curve_adc <= 180) {
		*next_curve_rise = 223; //was LIN25
		*next_curve_fall = 223; //was LIN25
	} else if (curve_adc <= 195) {
		*next_curve_rise = PureCurves::LOG;
		*next_curve_fall = PureCurves::LOG;
	} else if (curve_adc <= 210) {
		*next_curve_rise = PureCurves::LOG;
		*next_curve_fall = 191; // was LIN50
	} else if (curve_adc <= 225) {
		*next_curve_rise = PureCurves::LOG;
		*next_curve_fall = PureCurves::LIN;
	} else if (curve_adc <= 240) {
		*next_curve_rise = PureCurves::LOG;
		*next_curve_fall = 64; //was EXP50
	} else if (curve_adc <= 255) {
		*next_curve_rise = PureCurves::LOG;
		*next_curve_fall = PureCurves::EXPO;
	}
}

} // namespace MetaModule::PEG