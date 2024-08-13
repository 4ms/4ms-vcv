#include "calibration.hh"
#include "debounced_digins.h"
#include "envelope_calcs.h"
#include "peg_base.hh"
#include "settings.h"
#include "util/math.hh"

namespace MetaModule::PEG
{

//Settings:
#define USER_INPUT_POLL_TIME 80

#define ADC_DRIFT 16
#define DIV_ADC_HYSTERESIS 16

void PEGBase::init_params(void) {
	//Todo: store shift value in flash
	shift = 2048; //=settings.shift_value
	offset = 0;
	scale = 0;
}

void PEGBase::update_adc_params(uint8_t force_params_update) {

	// every 60us or so
	if (force_params_update || ++poll_user_input > USER_INPUT_POLL_TIME) {
		//every 500us or so
		poll_user_input = 0;

		if (read_shape_scale_offset()) {
			auto old_skew = m.skew;
			calc_skew_and_curves(skew, shape, &m.skew, &m.next_curve_rise, &m.next_curve_fall);
			calc_rise_fall_incs(&m);
			if (old_skew != m.skew) {
				update_env_tracking(&m);
				reset_transition_counter();
			}
		}

		if (m.env_state != TRANSITION /*|| !m.envelope_running*/)
		//Todo: remove !m.envelope_running and test. if m.env_state==TRANSITION, then m.envelope_running is true
		{
			int8_t new_clock_divider_amount = read_divmult();
			if (new_clock_divider_amount != 0) {
				update_clock_divider_amount(&m, new_clock_divider_amount);
				calc_rise_fall_incs(&m);

				update_env_tracking(&m);
				reset_transition_counter();
			}
		}

		//tranistion will happen 100 * 0.5ms = 50ms after clk divider amount stops changing
		if (check_to_start_transition()) {
			do_start_transition(&m);
		}
	} else {
		if (++oversample_wait_ctr > 4) {
			oversample_wait_ctr = 0;
			condition_analog();
		}
	}
}

// Reads Scale, Offset, and Shape CV and pots
// Updates global vars: shift, shape, offset
// Returns 1 if shape changed, 0 if not
uint8_t PEGBase::read_shape_scale_offset(void) {
	uint8_t update_risefallincs = 0;

	{
		int16_t adj_scale = analog[POT_SCALE].lpf_val + settings.center_detent_offset[DET_SCALE];
		scale = MathTools::plateau(adj_scale, SCALE_PLATEAU_WIDTH, 2048);
	}

	{
		int16_t adj_offset = analog[POT_OFFSET].lpf_val + settings.center_detent_offset[DET_OFFSET];
		int16_t tmp = MathTools::plateau(adj_offset, OFFSET_PLATEAU_WIDTH, 2048);
		if (is_pressed(CYCLE_BUTTON) && (MathTools::diff(cycle_latched_offset, analog[POT_OFFSET].lpf_val) > 40))
			adjusting_shift_mode = true;

		if (adjusting_shift_mode)
			// This variable is not used anymore
			// instead the settings value is used directly
			shift = 2048 + tmp;
		else
			//offset must go to zero at max
			offset = tmp - (2048 - OFFSET_PLATEAU_WIDTH / 2 - 4 /*wiggle room*/);
	}

	{
		int16_t shape_cv = MathTools::plateau(2048 - analog[CV_SHAPE].lpf_val, SHAPECV_PLATEAU_WIDTH, 0);
		int16_t shape_total = shape_cv + analog[POT_SHAPE].lpf_val + settings.center_detent_offset[DET_SHAPE];
		shape_total = MathTools::constrain(shape_total, (int16_t)0, (int16_t)4095);
		if (MathTools::diff((uint16_t)shape_total, shape) > ADC_DRIFT) {
			shape = shape_total;
			update_risefallincs = 1;
		}
	}

	{
		int16_t t_skew = analog[POT_SKEW].lpf_val + settings.center_detent_offset[DET_SKEW];
		t_skew = MathTools::constrain(t_skew, (int16_t)0, (int16_t)4095);
		if (MathTools::diff((uint16_t)t_skew, skew) > ADC_DRIFT) {
			skew = t_skew;
			update_risefallincs = 1;
		}
	}

	return update_risefallincs;
}

// Reads Divmult pot and cv
// returns updated clock divider
// amount or 0 if no change
int8_t PEGBase::read_divmult(void) {

	int16_t total_adc;
	int8_t t_clock_divider_amount = 1;
	int8_t hys_clock_divider_amount = 0;
	int8_t new_clock_divider_amount = 0;

	int16_t cv = MathTools::plateau(2048 - analog[CV_DIVMULT].lpf_val, DIVMULTCV_PLATEAU_WIDTH, 0);

	total_adc = MathTools::constrain(cv + analog[POT_DIVMULT].lpf_val, 0, 4095);

	uint16_t d = MathTools::diff(total_adc, last_total_adc);

	if (d > DIV_ADC_HYSTERESIS) {
		last_total_adc = total_adc;
		new_clock_divider_amount = get_clk_div_nominal(total_adc);
		if (last_clock_divider_amount == new_clock_divider_amount)
			return 0; //ADC jumped, but no change in clock divider amount, so do nothing

		last_clock_divider_amount = new_clock_divider_amount;

	} else if (d != 0) {
		t_clock_divider_amount = get_clk_div_nominal(total_adc);

		if (t_clock_divider_amount == last_clock_divider_amount)
			return 0; //clock_divider_amount has not changed, do nothing

		if (t_clock_divider_amount > last_clock_divider_amount) {
			int16_t tmp = total_adc + DIV_ADC_HYSTERESIS;
			hys_clock_divider_amount = get_clk_div_nominal(tmp);
		}
		if (t_clock_divider_amount < last_clock_divider_amount) {
			int16_t tmp = total_adc - DIV_ADC_HYSTERESIS;
			hys_clock_divider_amount = get_clk_div_nominal(tmp);
		}

		if (hys_clock_divider_amount == t_clock_divider_amount) {
			new_clock_divider_amount = t_clock_divider_amount;
			last_clock_divider_amount = new_clock_divider_amount;
			last_total_adc = total_adc;
		}
	}
	return new_clock_divider_amount;
}

void PEGBase::update_env_tracking(struct PingableEnvelope *e) {
	if (e->envelope_running && e->sync_to_ping_mode)
		e->tracking_changedrisefalls = 1;

	e->async_env_changed_shape = 1;
}

void PEGBase::update_clock_divider_amount(struct PingableEnvelope *e, int16_t new_clock_divider_amount) {
	e->clock_divider_amount = new_clock_divider_amount;

	if (clk_time) {
		if (e->ping_div_ctr < 0)
			e->ping_div_ctr = 0;
		if (e->ping_div_ctr > e->clock_divider_amount)
			e->ping_div_ctr = e->clock_divider_amount;

		e->div_clk_time = get_clk_div_time(new_clock_divider_amount, clk_time);
	}
}

} // namespace MetaModule::PEG
