/*
 * pwm.h
 */

#pragma once
#include <stddef.h>

namespace MetaModule::PEG
{

enum PwmOutputs {
	PWM_ENVA_R,
	PWM_ENVA_G,
	PWM_ENVA_B,

	PWM_ENVB_R,
	PWM_ENVB_G,
	PWM_ENVB_B,

	PWM_CYCLEBUT_R,
	PWM_CYCLEBUT_G,
	PWM_CYCLEBUT_B,

	PWM_PINGBUT_R,
	PWM_PINGBUT_G,
	PWM_PINGBUT_B,

	PWM_EOF_LED,

	NUM_PWMS
};

}
