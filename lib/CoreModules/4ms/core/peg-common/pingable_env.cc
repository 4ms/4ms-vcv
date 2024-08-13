#include "peg_base.hh"

namespace MetaModule::PEG
{

void PEGBase::init_pingable_env(struct PingableEnvelope *e) {
	e->divpingtmr = 0;
	e->ping_div_ctr = 0;
	e->clock_divider_amount = 1;
	e->div_clk_time = 0;
	e->rise_time = 0;
	e->fall_time = 0;
	e->rise_inc = 0;
	e->fall_inc = 0;
	e->curve_rise = 127;
	e->next_curve_rise = 127;
	e->curve_fall = 127;
	e->next_curve_fall = 127;
	e->cur_curve = 127;
	e->envelope_running = 0;
	e->env_state = WAIT;
	e->next_env_state = WAIT;
	e->accum = 0;
	e->segphase = 0;
	e->cur_val = 0;
	e->sync_to_ping_mode = 0;
	e->reset_now_flag = 0;
	e->ready_to_start_async = 0;
	e->async_phase_diff = 0;
	e->async_env_changed_shape = 1;
	e->tracking_changedrisefalls = 0;
	e->transition_inc = 0;
	e->transition_ctr = 0;
	e->accum_endpoint = 0;
	e->outta_sync = 0;
}

void PEGBase::copy_skew_and_curves(struct PingableEnvelope *dst, struct PingableEnvelope *src) {
	dst->skew = src->skew;
	dst->next_curve_rise = src->next_curve_rise;
	dst->next_curve_fall = src->next_curve_fall;
}

void PEGBase::copy_rise_fall_incs(struct PingableEnvelope *dst, struct PingableEnvelope *src) {
	dst->fall_time = src->fall_time;
	dst->rise_time = src->rise_time;
	dst->rise_inc = src->rise_inc;
	dst->fall_inc = src->fall_inc;
}

}
