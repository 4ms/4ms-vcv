#include "peg_base.hh"

namespace MetaModule::PEG
{

void PEGBase::update_pwm(uint32_t pwmval, enum PwmOutputs pwmnum) {
	if (pwmnum >= NUM_PWMS)
		return;

    pwm_vals[pwmnum] = pwmval;
}

void PEGBase::init_pwm(void) {

}

}
