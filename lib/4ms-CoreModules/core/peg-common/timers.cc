//Todo: replace as many as possible with HAL_GetTicks() modulo some value
//
#include "peg_base.hh"

namespace MetaModule::PEG
{

void PEGBase::inc_tmrs(void) {
	systmr++;
	tapouttmr++;
	tapintmr++;
	pingtmr++;
	trigouttmr++;
	trigouttmr_secondary++;
}

}
