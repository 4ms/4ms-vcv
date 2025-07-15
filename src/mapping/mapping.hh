#pragma once
#include <rack.hpp>

// This is the same as patch-serial's MappedKnob, except for 64-bit moduleId (which VCV uses)
struct Mapping {
	int64_t module_id = -1;
	int param_id = -1;
	std::string alias_name = "";

	float range_min = 0.f;
	float range_max = 1.f;

	uint8_t curve_type = 0;

	void clear() {
		module_id = -1;
		param_id = -1;
		// text = "";
		range_min = 0.f;
		range_max = 1.f;
		curve_type = 0;
	}
};
