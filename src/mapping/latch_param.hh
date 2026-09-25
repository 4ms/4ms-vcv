#pragma once
#include "patch/patch.hh"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace MetaModule
{

// These match MetaModule firmware (params/toggle_param.hh). Values are normalized (0..1)

// Returns max if cur_val is closer to min, otherwise returns min
inline float toggle_value(float cur_val, float min, float max) {
	if (std::abs(cur_val - min) < std::abs(cur_val - max))
		return max;
	else
		return min;
}

// Returns the next position of a param with num_pos discrete positions (value == pos / (num_pos - 1))
// within the min/max range, wrapping around. Steps from min towards max (so max < min steps backwards).
// Falls back to toggling if the param is not multi-position or the range spans fewer than 2 positions.
inline float step_value(float cur_val, float min, float max, unsigned num_pos) {
	if (num_pos < 2)
		return toggle_value(cur_val, min, max);

	const float steps = num_pos - 1;

	// Min/Max are set in 1% increments, so allow for rounding
	const float tolerance = 0.006f * steps;
	const int lo = std::ceil(std::min(min, max) * steps - tolerance);
	const int hi = std::floor(std::max(min, max) * steps + tolerance);

	if (hi <= lo)
		return toggle_value(cur_val, min, max);

	const bool reversed = max < min;
	const int cur = std::lround(cur_val * steps);

	int next;
	if (cur < lo || cur > hi)
		next = reversed ? hi : lo;
	else if (reversed)
		next = (cur == lo) ? hi : cur - 1;
	else
		next = (cur == hi) ? lo : cur + 1;

	return next / steps;
}

// Buttons mapped to params with num_pos > 2 default to Step mode. Everything else defaults to Normal.
// Values match MappedKnob::CurveType
inline uint8_t default_curve_type(bool is_button, unsigned num_pos) {
	return (is_button && num_pos > 2) ? MappedKnob::CurveType::Cycle : MappedKnob::CurveType::Normal;
}

} // namespace MetaModule
