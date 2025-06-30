#pragma once

#include <cstdint>

struct CableMap {
	int outputJackId = 0;
	int inputJackId = 0;
	int64_t outputModuleId = 0;
	int64_t inputModuleId = 0;
	uint16_t lv_color_full = 0;

	bool isSameJack(const CableMap &other) const {
		return (outputJackId == other.outputJackId) && (outputModuleId == other.outputModuleId);
	}
};
