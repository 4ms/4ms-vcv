#pragma once
#include <cstdint>

static inline constexpr float InputLowRangeVolts = -10.27f;
static inline constexpr int32_t InputLowRangeMillivolts = -10270;
static inline constexpr float InputHighRangeVolts = 10.31f;
static inline constexpr int32_t InputHighRangeMillivolts = 10310;
static inline constexpr float InputRangeVolts = InputHighRangeVolts - InputLowRangeVolts;
static inline constexpr float InputRangeCenterVolts = (InputHighRangeVolts + InputLowRangeVolts) / 2.f;

static inline constexpr float OutputLowRangeVolts = -8.59f;
static inline constexpr float OutputHighRangeVolts = 8.59f;
static inline constexpr float OutputRangeVolts = OutputHighRangeVolts - OutputLowRangeVolts;
static inline constexpr float OutputRangeCenterVolts = (OutputHighRangeVolts + OutputLowRangeVolts) / 2.f;
