#include "peg_base.hh"

#include "flash_user.hh"
#include "math_util.h"

#include "settings.h"
#include "util/math.hh"

namespace MetaModule::PEG
{

PEGBase::PEGBase(EnvelopeCalcsBase *env_calcs)
	:envelope_calcs{env_calcs}
{
	init_pingable_env(&m);
	init_dig_inouts();

	eor_off();
	eof_off();
	hr_off();
	tapclkout_off();

	init_analog_conditioning();

	init_pwm();
	all_lights_off();

	init_debouncer();

	read_settings();
	adjust_palette();

	init_params();

	apply_settings();

	last_tapin_time = 0;

}

void PEGBase::apply_settings() {
	m.sync_to_ping_mode = false;
	m.async_env_changed_shape = 0;
	m.ready_to_start_async = true;

	if (settings.start_clk_time) {
		clk_time = settings.start_clk_time;
		m.div_clk_time = settings.start_clk_time;
		tapout_clk_time = clk_time;
		using_tap_clock = 1;
		tapintmr = 0;
		tapouttmr = 0;
		calc_div_clk_time(&m, clk_time);
	}

	if (settings.start_cycle_on) {
		cycle_but_on = 1;
		set_rgb_led(LED_CYCLE, c_ORANGE);
		m.envelope_running = 1;
		using_tap_clock = 1;
		tapout_clk_time = clk_time;
	} else {
		cycle_but_on = 0;
		set_rgb_led(LED_CYCLE, c_OFF);
		m.envelope_running = 0;
	}
}

void PEGBase::update()
{
	read_ping_button();
	read_trigjacks();
	read_cycle_button();
	check_reset_envelopes();

	update_tap_clock();
	read_ping_clock();
	update_adc_params(force_params_update);
	force_params_update = 0;

	handle_trigout_trigfall();
	handle_trigout_secondary_trigfall();

	// handle_system_mode(m.sync_to_ping_mode);

	settings.start_clk_time = clk_time;
	settings.start_cycle_on = cycle_but_on;
}

void PEGBase::pingEdgeIn()
{
	ping_irq_timestamp = pingtmr;
	clockbus_on();
	pingtmr = 0;
	using_tap_clock = 0;
}

void PEGBase::set_sync_mode(bool mode) {
	m.sync_to_ping_mode = mode;
}

void PEGBase::read_ping_button() {
	if (toggled_sync_mode)
		return;

	if (is_pressed(PING_BUTTON)) {
		uint32_t now = tapintmr;

		if (just_pressed(PING_BUTTON)) {

			if (is_pressed(CYCLE_BUTTON)) {
				m.sync_to_ping_mode = !m.sync_to_ping_mode;
				toggled_sync_mode = true;
				return;
			}

			using_tap_clock = 1;

			if (last_tapin_time && (MathTools::diff(last_tapin_time, now) < (last_tapin_time >> 1))) {
				clk_time = (now >> 1) + (last_tapin_time >> 1);
			} else {
				clk_time = now;
				last_tapin_time = now;
			}

			tapout_clk_time = clk_time;
			tapintmr = 0;
			tapouttmr = 0;

			calc_div_clk_time(&m, clk_time);

			force_transition();
			force_params_update = 1;
		} else {
			if (now > HOLDTIMECLEAR) { // button has been down for more than 2 seconds
				if (using_tap_clock) {
					stop_envelope(&m);
					clk_time = 0;
					using_tap_clock = 0;
				}
				tapout_clk_time = 0;
				last_tapin_time = 0;
				tapouttmr = 0;
				ping_led_off();
			} else
				ping_led_on();
		}
	}

	if (just_released(PING_BUTTON)) {
		ping_led_off();
	}
}

void PEGBase::handle_qnt_trig(struct PingableEnvelope *e) {
	e->triga_down = 0;
	e->trigq_down = 1;
	e->sync_to_ping_mode = 1;
	e->reset_nextping_flag = 1;

	if (QNT_REPHASES_WHEN_CYCLE_OFF || cycle_but_on || !e->envelope_running) {
		e->ping_div_ctr = e->clock_divider_amount; // make sure divided clock envelopes start from
												   // beginning on next ping

		// TODO: this used to check rise_time instead of limit_skew. Change it back?
		// if (e->rise_time > 0x10) // was 0x1000
		if (settings.limit_skew && e->cur_val > 100) //~200mV max (Scale at max)
			e->outta_sync = 1;
	}
	e->tracking_changedrisefalls = 0;
	e->curve_rise = e->next_curve_rise;
	e->curve_fall = e->next_curve_fall;
}

void PEGBase::handle_async_trig(struct PingableEnvelope *e) {
	e->triga_down = 1;
	e->trigq_down = 0;
	e->sync_to_ping_mode = 0;
	e->reset_now_flag = 1;
	e->ready_to_start_async = 0;

	// Do a transition if env is above a certain level and Skew Limit is enabled
	if (settings.limit_skew && e->cur_val > 100) //~200mV max (Scale at max)
		e->outta_sync = 1;

	// TODO: this used to check rise_time instead of limit_skew. Change it back?
	// if (e->rise_time > 0x10) // or 0x1000
	// do an immediate fall if rise_time is fast
	// otherwise set the outta_sync flag which works to force
	// a slew limited transition to zero

	e->async_phase_diff = e->divpingtmr;
}

void PEGBase::read_trigjacks() {
	if (just_pressed(TRIGGER_JACK)) {
		if (settings.trigin_function == TRIGIN_IS_QNT)
			handle_qnt_trig(&m);
		else
			handle_async_trig(&m);
	}

	if (just_released(TRIGGER_JACK)) {
		m.triga_down = 0;
		m.trigq_down = 0;
	}

	if (just_pressed(CYCLE_JACK)) {
		if (settings.cycle_jack_behavior == CYCLE_JACK_BOTH_EDGES_TOGGLES_QNT)
			m.sync_to_ping_mode = 1;
		do_toggle_cycle = 1;
	}

	if (just_released(CYCLE_JACK)) {

		if (settings.cycle_jack_behavior == CYCLE_JACK_BOTH_EDGES_TOGGLES ||
			settings.cycle_jack_behavior == CYCLE_JACK_BOTH_EDGES_TOGGLES_QNT)
			do_toggle_cycle = 1;
	}
}

void PEGBase::update_trigout() {
	if (m.env_state == RISE) {
		if ((m.accum >> 19) >= 2048)
			hr_on();
		else
			hr_off();
		eor_off();
		eof_on();
	} else if (m.env_state == FALL) {
		if ((m.accum >> 19) < 2048)
			hr_off();
		else
			hr_on();
		eor_on();
		eof_off();
	}
}

void PEGBase::read_cycle_button() {
	if (just_pressed(CYCLE_BUTTON)) {
		cycle_latched_offset = analog[POT_OFFSET].lpf_val;
	}

	auto released_cycle = just_released(CYCLE_BUTTON);

	if (released_cycle && adjusting_shift_mode) {
		adjusting_shift_mode = false;
		released_cycle = false;
	}

	if (released_cycle && toggled_sync_mode) {
		toggled_sync_mode = false;
		released_cycle = false;
	}

	if (released_cycle || do_toggle_cycle) {
		do_toggle_cycle = 0;

		if (cycle_but_on == 0) {
			cycle_but_on = 1;
			set_rgb_led(LED_CYCLE, c_ORANGE);

			if (clk_time > 0) {
				calc_div_clk_time(&m, clk_time);

				start_envelope(&m);

				update_trigout();
			}
		} else {
			cycle_but_on = 0;
			set_rgb_led(LED_CYCLE, c_OFF);
		}
	}
}

void PEGBase::ping_led_off() {
	set_rgb_led(LED_PING, c_OFF);
	div_ping_led = 0;
}

void PEGBase::ping_led_on() {
	set_rgb_led(LED_PING, m.sync_to_ping_mode ? c_CYAN : c_WHITE);
	div_ping_led = 1;
}

// Todo: this should be done when divpingtmr is updated, or when div_clk_time is
// updated void resync_on_divpingtmr()
void PEGBase::check_reset_envelopes() {
	check_restart_async_env(&m);

	if (div_ping_led && (m.divpingtmr >= (m.div_clk_time >> 1))) {
		ping_led_off();
	}

	if (m.div_clk_time) {
		if ((!div_ping_led) && (m.divpingtmr > m.div_clk_time)) { // Todo: why do we have !div_ping_led
																  // && ( > ). Do we need that?
			m.divpingtmr = m.divpingtmr - m.div_clk_time;
			sync_env_to_clk(&m);
			ping_led_on();
		}

	} else
		ping_led_off();
}

// Todo: this only needs to be done when tapouttmr updates
void PEGBase::update_tap_clock() {
	if (tapout_clk_time) {
		if (tapouttmr >= tapout_clk_time) {
			tapouttmr = 0;
			if (using_tap_clock) {
				clockbus_on();
				// This was causing issue #7
				// if (m.clock_divider_amount <= 1)
				// 	m.divpingtmr = 0;
				// DebugReset = 2;

				got_tap_clock = 1;
			}
			tapclkout_on();
		} else if (tapouttmr >= (tapout_clk_time >> 1))
			tapclkout_off();
	}
}

void PEGBase::read_ping_clock() {
	if (got_tap_clock || ping_irq_timestamp) {
		if (ping_irq_timestamp) {
			uint32_t prev_clk_time = clk_time;
			clk_time = ping_irq_timestamp;

			if (prev_clk_time) {
				uint32_t delta = MathTools::diff(clk_time, prev_clk_time);
				if (delta > (prev_clk_time >> 3)) //>12.5%
				{
					force_transition();
					m.tracking_changedrisefalls = 1;
					force_params_update = 1;
				} else if (delta > 100) {
					m.tracking_changedrisefalls = 1;
					reset_transition_counter();
				}
			}
		}
		ping_irq_timestamp = 0;
		got_tap_clock = 0;

		if (!using_tap_clock)
			last_tapin_time = 0;

		if (m.clock_divider_amount <= 1) {
			ping_led_on();
			// TODO: test if we need to reset divpingtmr in sync mode (we don't in async mode)
			if (m.sync_to_ping_mode) {
				m.divpingtmr = 0;
			}
		}

		calc_div_clk_time(&m, clk_time);

		if (resync_on_ping(&m))
			ping_led_on();

	} else {
		/*	If we haven't received a ping within 2x expected clock time (that is,
	   clock stopped or slowed to less than half speed) we should stop the ping
	   clock. Or switch to the Tap clock if it's running and we have Tap Clock
	   Output on EOF
	*/
		if (clk_time && !settings.free_running_ping && !using_tap_clock) {
			const uint32_t ext_ping_timeout = clk_time * 2;
			if (pingtmr >= ext_ping_timeout) {
				pingtmr = 0;

				if (tapout_clk_time && (settings.trigout_function == TRIGOUT_IS_TAPCLKOUT)) {
					// switch to tap clock
					using_tap_clock = 1;
					clk_time = tapout_clk_time;

					calc_div_clk_time(&m, clk_time);

					m.reset_now_flag = 1;
				} else {
					stop_envelope(&m);
					clk_time = 0;
				}
				ping_led_off();
			}
		}
	}

	if (clk_time) {
		auto tm = using_tap_clock ? tapouttmr : pingtmr;
		if (tm >= (clk_time >> 1))
			clockbus_off();
	}
}

}
