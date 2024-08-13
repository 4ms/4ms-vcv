#pragma once
#include <cstring>

namespace EnOsc
{

template<typename T>
struct Persistent : T {
	Persistent(T *data, T const &default_data) {
		memcpy(data, &default_data, sizeof(T));
	}
	void Save() {
	}
};

template<typename T>
using WearLevel = T;

template<int, typename T>
using FlashBlock = T;

} // namespace EnOsc
