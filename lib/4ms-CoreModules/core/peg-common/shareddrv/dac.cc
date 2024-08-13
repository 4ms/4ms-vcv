#include "peg_base.hh"

namespace MetaModule::PEG
{

void PEGBase::dac_out(enum DACs dac, uint16_t val) {
	if (dac == DAC_ENVA) {
		dac_vals[dac] = 4095 - val;
	} else if (dac == DAC_ENVB) {
		dac_vals[dac] = val;
	}
}

}
