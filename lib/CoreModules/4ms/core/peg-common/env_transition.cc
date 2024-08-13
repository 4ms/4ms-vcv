#include "peg_base.hh"
#include "dig_inout_pins.hh"
#include "envelope_calcs.h"
#include "util/math.hh"

namespace MetaModule::PEG
{

const uint32_t NUM_ADC_CYCLES_BEFORE_TRANSITION = 200; //200 is about 100ms

constexpr uint32_t TransitionPeriod = 512;
constexpr uint32_t TransitionPeriodBitShift = MathTools::Log2<TransitionPeriod>::val;

void PEGBase::reset_transition_counter() {
	didnt_change_divmult = 1;
	// DigIO::DebugOut::high();
}

void PEGBase::force_transition() {
	didnt_change_divmult = NUM_ADC_CYCLES_BEFORE_TRANSITION;
}

bool PEGBase::check_to_start_transition() {
	if (didnt_change_divmult > 0) {
		didnt_change_divmult++;
		if (didnt_change_divmult >= NUM_ADC_CYCLES_BEFORE_TRANSITION) {
			didnt_change_divmult = 0;
			return true;
		}
	}
	return false;
}

void PEGBase::do_start_transition(PingableEnvelope *e) {
	uint32_t elapsed_time;
	if (!e->div_clk_time)
		return;

	if (!e->sync_to_ping_mode) {
		e->async_env_changed_shape = 1;
	} else {
		e->tracking_changedrisefalls = 0;

		if (using_tap_clock)
			elapsed_time = tapouttmr;
		else
			elapsed_time = pingtmr;

		if (e->clock_divider_amount <= 1) //Multiplying
		{
			while (elapsed_time > e->div_clk_time)
				elapsed_time -= e->div_clk_time;
		} else //Dividing
		{
			if (e->envelope_running) {
				e->ping_div_ctr = calc_divided_ping_div_ctr(e, e->env_state);

				while (elapsed_time >= clk_time)
					elapsed_time -= clk_time;
				elapsed_time += e->ping_div_ctr * clk_time;
			} else //envelope not running
			{
				e->ping_div_ctr = e->clock_divider_amount; //the next ping will be the one that the envelope starts on
				while (elapsed_time <= e->div_clk_time)
					elapsed_time += clk_time;
				elapsed_time -= clk_time;
			}
		}

		e->divpingtmr = elapsed_time; // was >>8;

		if (e->envelope_running) {
			elapsed_time += TransitionPeriod + 6; //offset to account for transition period + 6 overflows during calc
			start_transition(e, elapsed_time);
			e->outta_sync = 1;
			e->ready_to_start_async = 1;
		}

		if ((e->clock_divider_amount > 1) && e->envelope_running) {
			e->ping_div_ctr = calc_divided_ping_div_ctr(e, e->next_env_state);
		}
	}
}

// FIXME: if elapsed_time is not at least TransisionPeriod, then does the TransitionPeriodBitShift math work?
void PEGBase::start_transition(PingableEnvelope *e, uint32_t elapsed_time) {
	if (elapsed_time > e->div_clk_time)
		elapsed_time -= e->div_clk_time;

	uint16_t dacval_endpoint;
	if (elapsed_time <= e->rise_time) {
		uint64_t time_tmp = ((uint64_t)elapsed_time) << 12; //Fixed-point U12.0
		uint16_t segphase_endpoint = time_tmp / e->rise_time;
		if (segphase_endpoint > 4095)
			segphase_endpoint = 4095;
		dacval_endpoint = calc_curve(segphase_endpoint, e->next_curve_rise);
		e->accum_endpoint = segphase_endpoint << 19;
		e->next_env_state = RISE;
	} else {
		elapsed_time -= e->rise_time;
		uint64_t time_tmp = ((uint64_t)elapsed_time) << 12; //Fixed-point U12.0
		uint16_t segphase_endpoint = time_tmp / e->fall_time;
		if (segphase_endpoint >= 4095) {
			segphase_endpoint = 0;
			dacval_endpoint = 0;
			e->accum_endpoint = 0;
		} else {
			segphase_endpoint = 4095 - segphase_endpoint;
			dacval_endpoint = calc_curve(segphase_endpoint, e->next_curve_fall);
			e->accum_endpoint = segphase_endpoint << 19;
		}
		e->next_env_state = FALL;
	}

	//split (segphase_endpoint - segphase) into 128 pieces (>>7), but in units of accum (<<19) ===> <<12
	//Todo: use signed ints to simplify this
	if (dacval_endpoint > e->cur_val)
		e->transition_inc = (dacval_endpoint - e->cur_val) << (19 - TransitionPeriodBitShift); //12;
	else {
		e->transition_inc = (e->cur_val - dacval_endpoint) << (19 - TransitionPeriodBitShift); //12;
		e->transition_inc = -1 * e->transition_inc;
	}

	e->cur_curve = LIN;
	e->accum = e->cur_val << 19;

	e->env_state = TRANSITION;
	e->transition_ctr = TransitionPeriod;
}

int8_t PEGBase::calc_divided_ping_div_ctr(struct PingableEnvelope *e, enum envelopeStates envstate) {
	int8_t pdc;
	uint32_t rise_per_clk;
	uint32_t temp_u32;
	uint8_t temp_u8;

	if (envstate == RISE) {
		//Todo, test this, does it work?
		pdc = 0;
		if (e->rise_time > clk_time) {
			rise_per_clk = (e->rise_inc >> 8) * (clk_time >> 8);
			if (rise_per_clk < 32)
				rise_per_clk = 32;
			temp_u32 = 0;
			temp_u8 = 0;
			while ((temp_u32 <= 0x00007FF8) && (temp_u8 <= e->clock_divider_amount)) { //was temp_u32<0x10000000
				temp_u32 += rise_per_clk;
				temp_u8++;
				if ((e->accum >> 16) >= temp_u32)
					pdc++;
			}
		}
	} else if (envstate == FALL) {
		pdc = e->clock_divider_amount - 1;
		if (e->fall_time > clk_time) //otherwise leave pdc at the last step
		{
			rise_per_clk = (e->fall_inc >> 8) * (clk_time >> 8); //was fall_inc * (clk_time>>8);
			if (rise_per_clk < 32)
				rise_per_clk = 32;
			temp_u32 = 0;
			temp_u8 = 0;
			while ((temp_u32 <= 0x00007FF8) && (temp_u8 <= e->clock_divider_amount)) {
				temp_u32 += rise_per_clk;
				temp_u8++;
				if ((e->accum >> 16) >= temp_u32)
					pdc--;
			}
		}
	} else
		pdc = e->ping_div_ctr;

	return pdc;
}

}
