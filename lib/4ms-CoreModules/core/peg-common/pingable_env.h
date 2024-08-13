#pragma once
#include <stdint.h>

namespace MetaModule::PEG
{

enum envelopeStates {
	WAIT = 0,
	RISE = 1,
	SUSTAIN = 2,
	FALL = 3,
	TRANSITION = 4,
};

struct PingableEnvelope {
	uint32_t divpingtmr = 0;
	int8_t ping_div_ctr = 0;
	int8_t clock_divider_amount = 0;
	uint8_t reset_nextping_flag = 0;

	uint32_t div_clk_time = 0;
	uint32_t rise_time = 0;
	uint32_t fall_time = 0;
	uint32_t rise_inc = 0;
	uint32_t fall_inc = 0;

	uint8_t skew = 0;
	uint8_t curve_rise = 0;
	uint8_t curve_fall = 0;
	uint8_t cur_curve = 0;
	uint8_t next_curve_rise = 0;
	uint8_t next_curve_fall = 0;

	//env state
	char envelope_running = 0;
	envelopeStates env_state = envelopeStates::WAIT;
	envelopeStates next_env_state = envelopeStates::WAIT;

	//envelope phase
	uint32_t accum = 0;
	uint16_t segphase = 0;
	uint16_t cur_val = 0;

	//syncing/async to ping
	uint8_t sync_to_ping_mode = 0;
	char reset_now_flag = 0;
	char ready_to_start_async = 0;
	uint32_t async_phase_diff = 0;
	uint8_t async_env_changed_shape = 0;

	//transitioning
	char tracking_changedrisefalls = 0;
	int32_t transition_inc = 0;
	int32_t transition_ctr = 0;
	uint32_t accum_endpoint = 0;
	char outta_sync = 0;

	uint8_t trigq_down = 0;
	uint8_t triga_down = 0;
};

}
