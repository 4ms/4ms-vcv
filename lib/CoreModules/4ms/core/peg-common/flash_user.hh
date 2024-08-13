/*
 * flash_user.h
 */

#pragma once

#include "envelope_calcs.h"
#include <stdint.h>

namespace MetaModule::PEG
{

enum CycleJackBehaviors {
	CYCLE_JACK_RISING_EDGE_TOGGLES,
	CYCLE_JACK_BOTH_EDGES_TOGGLES_QNT,
	CYCLE_JACK_BOTH_EDGES_TOGGLES,

	NUM_CYCLEJACK_BEHAVIORS
};

// enum AuxTrigJackAssignment {
// 	AUX_ENV_TRIG,
// 	CYCLE_TOGGLE,

// 	NUM_AUX_TRIG_JACK_ASSIGNMENTS
// };

enum TrigOutFunctions {
	TRIGOUT_IS_ENDOFRISE,
	TRIGOUT_IS_ENDOFFALL,
	TRIGOUT_IS_HALFRISE,
	TRIGOUT_IS_TAPCLKOUT,

	NUM_TRIGOUT_FUNCTIONS
};

enum TrigInFunctions {
	TRIGIN_IS_ASYNC,
	TRIGIN_IS_ASYNC_SUSTAIN,
	TRIGIN_IS_QNT,

	NUM_TRIGIN_FUNCTIONS
};

enum CenterDetentPots {
	DET_SCALE,
	DET_OFFSET,
	DET_SHAPE,
	DET_SKEW,

	NUM_CENTER_DETENT_POTS
};

#define VALID_SETTINGS 0xC002

struct SystemSettings {
	uint16_t is_valid = 0;
	int16_t center_detent_offset[NUM_CENTER_DETENT_POTS] = {0};

	uint8_t limit_skew = 0;
	uint8_t free_running_ping = 0;
	uint8_t trigout_is_trig = 0;
	uint8_t trigout_secondary_is_trig = 0;
	TrigInFunctions trigin_function = TrigInFunctions(0);
	TrigOutFunctions trigout_function = TrigOutFunctions(0);
	TrigOutFunctions trigout_secondary_function = TrigOutFunctions(0);

	CycleJackBehaviors cycle_jack_behavior = CycleJackBehaviors(0);

	uint32_t start_clk_time = 0;
	uint8_t start_cycle_on = 0;

	uint16_t ping_cal_r = 0;
	uint16_t ping_cal_g = 0;
	uint16_t ping_cal_b = 0;
	uint16_t cycle_cal_r = 0;
	uint16_t cycle_cal_g = 0;
	uint16_t cycle_cal_b = 0;
	uint16_t unused1 = 0;
	uint16_t unused2 = 0;
	uint16_t unused3 = 0;
	uint16_t enva_cal_r = 0;
	uint16_t enva_cal_g = 0;
	uint16_t enva_cal_b = 0;
	uint16_t envb_cal_r = 0;
	uint16_t envb_cal_g = 0;
	uint16_t envb_cal_b = 0;

	int32_t shift_value = 0;
};

#define QNT_REPHASES_WHEN_CYCLE_OFF 0
#define CYCLE_REPHASES_DIV_PING 1

}
