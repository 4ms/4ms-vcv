#include "peg_base.hh"

#include "envelope_calcs.h"
#include "leds.h"
#include "settings.h"

namespace MetaModule::PEG
{

const uint32_t k_accum_max = (0xFFF << 19);
const uint32_t k_accum_min = (0x001 << 19);

void PEGBase::update_all_envelopes() {
	//@22kHz: 6.5us, every 45.5us, = 14.2%
	//@40kHz: 6.5us, every 25.0us = 26.0%
	// DEBUGON;
	m.divpingtmr = m.divpingtmr + 1;
	inc_tmrs();

	update_envelope(&m);
	output_env_val(m.cur_val);
	// DEBUGOFF;
}

void PEGBase::update_envelope(PingableEnvelope *e) {
	envelopeStates end_segment_flag = WAIT;
	uint8_t end_env_flag = 0;

	if ((clk_time == 0) || (e->div_clk_time == 0)) {
		e->envelope_running = 0;
		e->outta_sync = 0;
	}

	// DEBUGOFF;
	if (e->reset_now_flag)
		do_reset_envelope(e);

	if (!e->envelope_running) {
		eor_off();
		hr_off();
		eof_on();
		e->outta_sync = 0;
		return;
	}

	switch (e->env_state) {
		case (RISE):
			// DigIO::DebugOut::low();
			e->accum += e->rise_inc;
			e->segphase = e->accum >> 19;
			if (e->accum > k_accum_max) {
				e->accum = k_accum_max;
				e->segphase = 0x0FFF;
				if (e->triga_down && settings.trigin_function == TRIGIN_IS_ASYNC_SUSTAIN)
					end_segment_flag = SUSTAIN;
				else
					end_segment_flag = FALL;
			}
			e->cur_curve = e->curve_rise;
			if (e->segphase >= 2048)
				hr_on();
			else
				hr_off();
			eor_off();
			eof_on();
			break;

		case (SUSTAIN):
			// DigIO::DebugOut::low();
			eor_off();
			eof_off();
			hr_on();
			e->segphase = 0x0FFF;
			if (e->triga_down && settings.trigin_function == TRIGIN_IS_ASYNC_SUSTAIN) {
				e->accum = k_accum_max;
				e->async_env_changed_shape = 1;
			} else {
				end_segment_flag = FALL;
			}
			break;

		case (FALL):
			// DigIO::DebugOut::low();
			e->accum -= e->fall_inc;
			e->segphase = e->accum >> 19;

			if ((e->accum < k_accum_min) || (e->accum > k_accum_max)) {
				e->accum = 0;
				e->segphase = 0;
				end_env_flag = 1;
			}
			eor_on();
			eof_off();
			if (e->segphase < 2048)
				hr_off();
			else
				hr_on();
			e->cur_curve = e->curve_fall;
			break;

		case (TRANSITION):
			// DigIO::DebugOut::high();
			e->accum += e->transition_inc;
			if (e->accum < k_accum_min || (e->transition_inc == 0)) {
				//trans_inc==0 would technically be an error, so this gives us an out
				e->accum = 0;
				e->segphase = 0;
				e->transition_ctr = 1;
			} else if (e->accum > k_accum_max) {
				e->accum = k_accum_max;
				e->segphase = 0x0FFF;
				e->transition_ctr = 1;
			} else
				e->segphase = e->accum >> 19;

			if (e->transition_inc > 0) {
				eor_off();
				eof_on();
			} else {
				eor_on();
				eof_off();
			}

			e->transition_ctr -= 1;
			if (e->transition_ctr <= 0) {
				end_segment_flag = e->next_env_state;

				//FixMe: accum_endpoint should be segphase_endpoint << 19, so that
				//in the next curve when we do:
				//     accum = accum_endpoint + inc
				//     segphase = accum >> 19
				//     nextcurve_dacout = calc_curve(segphase),
				// then nextcurve_dacout should be close to the current dacout value
				// Since TRANSITION is linear, that means nextcurve_dacout should be close to the current segphase value
				// thus we should set accum based on the segphase of the synced ping env
				e->accum = e->accum_endpoint;

				//SPEG Fixme: This logic looks wrong, should it be if (outta_sync==2) ? otherwise outta_sync always is set to 0
				// if (e->outta_sync) //2 means we got to transistion from reset_now_flag
				// 	e->outta_sync = 0;
				// else if (e->outta_sync == 1)
				// 	e->outta_sync = 2;
				// else
				e->outta_sync = 0;
			}
			break;

		default:
			// DigIO::DebugOut::low();
			break;
	}
	e->cur_val = calc_curve(e->segphase, e->cur_curve);

	handle_env_segment_end(e, end_segment_flag);
	handle_env_end(e, end_env_flag);
}

void PEGBase::handle_env_segment_end(struct PingableEnvelope *e, envelopeStates end_segment_flag) {
	if (end_segment_flag) {
		if (end_segment_flag == FALL)
			e->curve_fall = e->next_curve_fall;

		if (end_segment_flag == RISE)
			e->curve_rise = e->next_curve_rise;

		if (end_segment_flag == SUSTAIN)
			e->curve_fall = e->next_curve_fall;

		e->env_state = end_segment_flag;
	}
}

void PEGBase::handle_env_end(struct PingableEnvelope *e, uint8_t end_env_flag) {
	if (end_env_flag) {
		eof_on();
		eor_off(); //should already be OFF, but make sure
		hr_off();

		e->curve_rise = e->next_curve_rise;
		e->curve_fall = e->next_curve_fall;

		//Loop if needed
		if (cycle_but_on || e->trigq_down || e->reset_nextping_flag) {
			//Todo: SPEG code changed order here, check for race conditions
			if (e->sync_to_ping_mode)
				e->reset_nextping_flag = 0;
			else {
				e->ready_to_start_async = 1;
				if (e->async_env_changed_shape) //if we altered the waveshape, then re-calc the landing spot
					e->async_phase_diff = e->divpingtmr;
				e->async_env_changed_shape = 0;
			}
			e->envelope_running = 1;
			e->env_state = RISE;
		} else {
			e->envelope_running = 0;
			e->env_state = WAIT;
			e->outta_sync = 0;
		}
	}
}

int32_t PEGBase::scale_shift_offset_env(uint16_t raw_env_val) {
	int32_t env = (int32_t)raw_env_val;
	constexpr auto kScaleRange = 4096; // - SCALE_PLATEAU_WIDTH;
	env = (((env + offset) * scale) / kScaleRange) + settings.shift_value;

	if (env > 4095)
		env = 4095;
	else if (env < 0)
		env = 0;

	return env;
}

void PEGBase::output_env_val(uint16_t rawA) {
	int32_t envA = scale_shift_offset_env(rawA);
	int32_t envB = rawA;

	dac_out(DAC_ENVA, envA);
	dac_out(DAC_ENVB, envB);

	if (!system_mode_active) {
		if (envA < 2048) {
			set_led_brightness(0, PWM_ENVA_B);
			set_led_brightness((2048 - envA) * 2, PWM_ENVA_R);
		} else {
			set_led_brightness((envA - 2048) * 2, PWM_ENVA_B);
			set_led_brightness(0, PWM_ENVA_R);
		}

		set_led_brightness(envB, PWM_ENVB_B);
	}
}

void PEGBase::do_reset_envelope(struct PingableEnvelope *e) {
	e->reset_now_flag = 0;

	if (e->cur_val < 0x0010)
		e->outta_sync = 0; // if we're practically at bottom, then consider us in sync and do an immediate transition

	// TODO: rename outta_sync => transition_requested and then use this:
	//if (e->envelope_running && e->transition_requested && e->div_clk_time >= 2000) //2000 is 20Hz
	//{
	//	//Offset to account for transition period
	//	uint32_t elapsed_time = 512 + 6;
	//	start_transition(e, elapsed_time);
	//} else {
	//	e->envelope_running = 1;
	//	e->env_state = RISE;
	//	e->accum = 0;
	//	eof_on();
	//	eor_off();
	//}

	if (!e->envelope_running || e->outta_sync == 0 || e->div_clk_time < 2000) //2000 is 20Hz
	{
		e->envelope_running = 1;
		e->env_state = RISE;
		e->accum = 0;
		eof_on();
		eor_off();
	} else {
		//Offset to account for transition period
		uint32_t elapsed_time = 512 + 6;
		start_transition(e, elapsed_time);
	}

	e->curve_rise = e->next_curve_rise;
	e->curve_fall = e->next_curve_fall;

	e->reset_nextping_flag = 0;
}

void PEGBase::check_restart_async_env(struct PingableEnvelope *e) {
	if (e->async_phase_diff > e->div_clk_time)
		e->async_phase_diff = 0; //fail-safe measure

	if (!e->sync_to_ping_mode && cycle_but_on && (e->divpingtmr >= e->async_phase_diff) &&
		!e->async_env_changed_shape && e->ready_to_start_async && !e->triga_down)
	{
		// In async mode, reset the envelope on the same phase each Cycle
		// This happens in Async mode when envelope hits bottom
		// and for Issue #8 and Issue #9
		e->reset_now_flag = 1;
		e->ready_to_start_async = 0;
	}
}

void PEGBase::sync_env_to_clk(struct PingableEnvelope *e) {
	if (!e->sync_to_ping_mode)
		e->ready_to_start_async = 1;
	else {
		if (e->reset_nextping_flag || cycle_but_on || e->trigq_down) {
			if (!e->tracking_changedrisefalls) {
				// In sync mode, on the ping we must reset the envelope (cause a transition, usually)
				e->reset_now_flag = 1;
				e->reset_nextping_flag = 0;
			}
		}
		e->ping_div_ctr = e->clock_divider_amount; //make sure it restarts the next ping
	}
}

uint8_t PEGBase::resync_on_ping(struct PingableEnvelope *e) {
	if (cycle_but_on || e->trigq_down || e->reset_nextping_flag || e->envelope_running) {
		if (e->clock_divider_amount > 1) //we're dividing the clock, so resync on every N pings
		{
			if (e->envelope_running && !e->tracking_changedrisefalls)
				e->ping_div_ctr++;

			if (e->sync_to_ping_mode && (e->ping_div_ctr >= e->clock_divider_amount)) {
				e->divpingtmr = 0;
				// DebugReset = 6;

				sync_env_to_clk(e);
				e->ping_div_ctr = 0;

				return 1;
			}

		} else {
			//TODO: this is the same logic as sync_env_to_clk() except for ping_div_ctr vs divpingtmr. Combine.

			//re-sync on every ping, since we're mult or = to the clock
			if (e->sync_to_ping_mode && !e->tracking_changedrisefalls &&
				(e->reset_nextping_flag || cycle_but_on || e->trigq_down))
				e->reset_now_flag = 1;

			e->reset_nextping_flag = 0;
			//FYI: reset_next_ping goes low only right after an envelope starts (on the ping, of course)

			// DebugReset = 5;
			e->divpingtmr = 0;
		}
	}
	return 0;
}

void PEGBase::start_envelope(struct PingableEnvelope *e) {
	if (!e->envelope_running) {
		if (e->sync_to_ping_mode)
			start_envelope_in_sync(e);
		else
			start_envelope_immediate(e);
	} else if (CYCLE_REPHASES_DIV_PING)
		e->ping_div_ctr = e->clock_divider_amount;
}

void PEGBase::start_envelope_immediate(struct PingableEnvelope *e) {
	e->envelope_running = 1;
	e->reset_now_flag = 1;
	e->ready_to_start_async = 0;
	e->async_phase_diff = e->divpingtmr;
}

void PEGBase::start_envelope_in_sync(struct PingableEnvelope *e) {
	//Todo: use floats
	uint64_t time_tmp = 0;
	uint32_t elapsed_time;

	e->envelope_running = 1;

	if (using_tap_clock)
		elapsed_time = tapouttmr;
	else
		elapsed_time = pingtmr;

	if (e->clock_divider_amount <= 1) {
		while (elapsed_time > e->div_clk_time)
			elapsed_time -= e->div_clk_time;
	} else {
		while (elapsed_time <= e->div_clk_time)
			elapsed_time += clk_time;
		elapsed_time -= clk_time;
	}

	if (elapsed_time <= e->rise_time) {
		time_tmp = ((uint64_t)elapsed_time) << 12;
		e->accum = time_tmp / e->rise_time;
		e->accum <<= 19;
		e->env_state = RISE;
		e->curve_rise = e->next_curve_rise;
	} else {
		elapsed_time = elapsed_time - e->rise_time;
		time_tmp = ((uint64_t)elapsed_time) << 12;
		e->accum = 4096 - (time_tmp / e->fall_time);
		e->accum <<= 19;
		e->env_state = FALL;
		e->curve_fall = e->next_curve_fall;
	}
	e->ping_div_ctr = e->clock_divider_amount;
}

void PEGBase::stop_envelope(struct PingableEnvelope *e) {
	e->env_state = WAIT;
	e->envelope_running = 0;
	e->divpingtmr = 0;
	e->div_clk_time = 0;
	e->accum = 0;

	//Todo: make sure the following doesn't mess up stopping tap clock
	//by holding ping down for long-press
	e->rise_time = 0;
	e->fall_time = 0;
	e->rise_inc = 0;
	e->fall_inc = 0;
}

}
