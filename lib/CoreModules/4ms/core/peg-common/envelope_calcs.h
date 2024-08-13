/*
 * envelope_calcs.h
 */

#pragma once

#include "pingable_env.h"
#include <cstdint>

namespace MetaModule::PEG
{

struct EnvelopeCalcsBase {
	virtual int8_t get_clk_div_nominal(uint16_t adc_val) = 0;
	virtual void
	calc_skew_and_curves(uint16_t skewadc, uint16_t shapeadc, uint8_t *skew, uint8_t *next_curve_rise, uint8_t *next_curve_fall) = 0;
};

enum PureSkews { RAMPUP = 0, SYM = 127, RAMPDOWN = 255 };
enum PureCurves { EXPO = 0, LIN = 127, LOG = 255 };


} // namespace MetaModule::PEG
