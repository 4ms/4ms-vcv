#pragma once

#include <cstdint>

struct CableMap {
	int sendingJackId = 0;
	int receivedJackId = 0;
	int64_t sendingModuleId = 0;
	int64_t receivedModuleId = 0;
	uint16_t lv_color_full = 0;

	bool isSameJack(const CableMap &other) const {
		return (sendingJackId == other.sendingJackId) && (sendingModuleId == other.sendingModuleId);
	}
};
