#pragma once

#include <cstdint>

struct ParamMap {
	float value = 0;
	int paramID = 0;
	int64_t moduleID = 0;

	bool isSameParam(ParamMap &other) {
		return (paramID == other.paramID) && (moduleID == other.moduleID);
	}
};
