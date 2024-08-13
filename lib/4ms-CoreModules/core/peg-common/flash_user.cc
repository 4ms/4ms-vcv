#include "peg_base.hh"
#include "calibration.hh"


namespace MetaModule::PEG
{

int PEGBase::write_settings(void) {
	// TODO: check return code
	return 0;
}

uint8_t PEGBase::read_settings(void) {
	// TODO: check return code

	// Always load default settings for now
	default_settings();

	return 0;
}

uint8_t PEGBase::check_settings_valid(void) {
	return settings.is_valid == VALID_SETTINGS;
}

void PEGBase::default_settings(void) {
	default_calibration();
	settings.limit_skew = 1;
	settings.free_running_ping = 1;
	settings.trigout_is_trig = 0;
	settings.trigout_secondary_is_trig = 0;
	settings.trigin_function = TRIGIN_IS_ASYNC;
	settings.trigout_function = TRIGOUT_IS_ENDOFFALL;
	settings.trigout_secondary_function = TRIGOUT_IS_ENDOFFALL;

	settings.cycle_jack_behavior = CYCLE_JACK_BOTH_EDGES_TOGGLES;

	settings.start_clk_time = 25000;
	settings.start_cycle_on = 1;
}

}
