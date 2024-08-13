#pragma once
#include <cstdint>

// #define TRIGDEBUGMODE

#define TICKS_PER_MS 1

#define HOLDTIMECLEAR 100000 //4800000
#define LIMIT_SKEW_TIME 150	 //100 = 2.25ms, 200 = 5ms, 500=12ms

#define SYSTEM_MODE_HOLD_TIME 130000
#define SYSTEM_MODE_EXIT_TIME 260000

#define TRIGOUT_TRIG_TIME 91
#define TRIGOUT_MIN_GATE_TIME 37

constexpr inline int32_t SCALE_PLATEAU_WIDTH = 100;
constexpr inline int32_t OFFSET_PLATEAU_WIDTH = 100;
constexpr inline int32_t SHAPECV_PLATEAU_WIDTH = 100;
constexpr inline int32_t SKEWCV_PLATEAU_WIDTH = 100;
constexpr inline int32_t DIVMULTCV_PLATEAU_WIDTH = 100;
