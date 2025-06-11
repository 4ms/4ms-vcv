#pragma once
#include "rack.hpp"

struct KnobSnappedParamQuantity : rack::SwitchQuantity {
	bool is_snapped_knob = true;
};
