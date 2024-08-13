#include "peg_base.hh"
#include "leds.h"
#include "settings.h"

namespace MetaModule::PEG
{

void PEGBase::trigout_on() {
	if (!trigout_high) {
		digio.EOJack.high();
		set_led_brightness(kMaxBrightness, PWM_EOF_LED);
		trigout_high = 1;
		trigouttmr = 0;
	}
}
void PEGBase::trigout_off() {
	if (!settings.trigout_is_trig && (trigouttmr > TRIGOUT_MIN_GATE_TIME)) {
		digio.EOJack.low();
		set_led_brightness(0, PWM_EOF_LED);
		trigouttmr = 0;
	}
	trigout_high = 0;
}

void PEGBase::trigout_secondary_on() {
	if (!trigout_secondary_high) {
		digio.EOJackSecondary.high();
		// set_led_brightness(kMaxBrightness, PWM_EOF_LED);
		trigout_secondary_high = 1;
		trigouttmr_secondary = 0;
	}
}
void PEGBase::trigout_secondary_off() {
	if (!settings.trigout_secondary_is_trig && (trigouttmr_secondary > TRIGOUT_MIN_GATE_TIME)) {
		digio.EOJackSecondary.low();
		// set_led_brightness(0, PWM_EOF_LED);
		trigouttmr_secondary = 0;
	}
	trigout_secondary_high = 0;
}


void PEGBase::handle_trigout_trigfall() {
	if (settings.trigout_is_trig && trigouttmr > TRIGOUT_TRIG_TIME) {
		digio.EOJack.low();
		set_led_brightness(0, PWM_EOF_LED);
		trigouttmr = 0;
	}
}

void PEGBase::handle_trigout_secondary_trigfall() {
	if (settings.trigout_secondary_is_trig && trigouttmr_secondary > TRIGOUT_TRIG_TIME) {
		digio.EOJackSecondary.low();
		// set_led_brightness(0, PWM_EOF_LED);
		trigouttmr_secondary = 0;
	}
}

//Todo: verify these are inlined by the compiler, otherwise explicity do it
void PEGBase::eor_on() {
	if (settings.trigout_function == TRIGOUT_IS_ENDOFRISE)
		trigout_on();
	if (settings.trigout_secondary_function == TRIGOUT_IS_ENDOFRISE)
		trigout_secondary_on();
}
void PEGBase::eor_off() {
	if (settings.trigout_function == TRIGOUT_IS_ENDOFRISE)
		trigout_off();
	if (settings.trigout_secondary_function == TRIGOUT_IS_ENDOFRISE)
		trigout_secondary_off();
}

void PEGBase::eof_on() {
	if (settings.trigout_function == TRIGOUT_IS_ENDOFFALL)
		trigout_on();
	if (settings.trigout_secondary_function == TRIGOUT_IS_ENDOFFALL)
		trigout_secondary_on();
}
void PEGBase::eof_off() {
	if (settings.trigout_function == TRIGOUT_IS_ENDOFFALL)
		trigout_off();
	if (settings.trigout_secondary_function == TRIGOUT_IS_ENDOFFALL)
		trigout_secondary_off();
}

void PEGBase::hr_on() {
	if (settings.trigout_function == TRIGOUT_IS_HALFRISE)
		trigout_on();
	if (settings.trigout_secondary_function == TRIGOUT_IS_HALFRISE)
		trigout_secondary_on();
}
void PEGBase::hr_off() {
	if (settings.trigout_function == TRIGOUT_IS_HALFRISE)
		trigout_off();
	if (settings.trigout_secondary_function == TRIGOUT_IS_HALFRISE)
		trigout_secondary_off();
}

void PEGBase::tapclkout_on() {
	if (settings.trigout_function == TRIGOUT_IS_TAPCLKOUT)
		trigout_on();
	if (settings.trigout_secondary_function == TRIGOUT_IS_TAPCLKOUT)
		trigout_secondary_on();
}
void PEGBase::tapclkout_off() {
	if (settings.trigout_function == TRIGOUT_IS_TAPCLKOUT)
		trigout_off();
	if (settings.trigout_secondary_function == TRIGOUT_IS_TAPCLKOUT)
		trigout_secondary_off();
}
void PEGBase::clockbus_on() {
	digio.ClockBusOut.high();
}
void PEGBase::clockbus_off() {
	digio.ClockBusOut.low();
}
}
