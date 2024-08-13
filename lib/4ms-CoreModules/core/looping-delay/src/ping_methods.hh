#pragma once
#include <cmath>
#include <cstdint>
#include <optional>

enum class PingMethod : uint8_t {
	// The value of each is how many flashes display in SystemMode
	IGNORE_FLAT_DEVIATION_10 = 0,
	IGNORE_PERCENT_DEVIATION = 1,
	ONE_TO_ONE = 2,
	MOVING_AVERAGE_2 = 3,
	LINEAR_AVERAGE_4 = 4,
	IGNORE_FLAT_DEVIATION_5,
	AVERAGE_2_IGNORE_DEVIATION_10,
	MOVING_AVERAGE_4,
	LINEAR_AVERAGE_8,
	EXPO_AVERAGE_4,
	EXPO_AVERAGE_8,

	NUM_PING_METHODS,
};

struct PingMethodAlgorithm {

	// Returns nullopt if time should not be updated (TODO: is this the best approach? Taken from DLD)
	static std::optional<uint32_t> filter(uint32_t oldtime, uint32_t newtime, PingMethod method) {
		using enum PingMethod;
		switch (method) {
			case ONE_TO_ONE:
				return newtime;
				break;

			case IGNORE_FLAT_DEVIATION_5:
				return ignore_flat_deviation<5>(oldtime, newtime);
				break;

			case IGNORE_FLAT_DEVIATION_10:
				return ignore_flat_deviation<10>(oldtime, newtime);
				break;

			case IGNORE_PERCENT_DEVIATION:
				return ignore_percent_deviation(oldtime, newtime);
				break;

			case AVERAGE_2_IGNORE_DEVIATION_10:
				return average_2_ignore_deviation<10>(oldtime, newtime);
				break;

			case MOVING_AVERAGE_4:
				return moving_average_4(newtime);
				break;

			case MOVING_AVERAGE_2:
				return moving_average_2(oldtime, newtime);
				break;

			case LINEAR_AVERAGE_4:
				return boxcar_average_4(oldtime, newtime);
				break;

			case LINEAR_AVERAGE_8:
				return boxcar_average_8(oldtime, newtime);
				break;

			case EXPO_AVERAGE_8:
				return expo_average_8(oldtime, newtime);
				break;

			case EXPO_AVERAGE_4:
				return expo_average_4(oldtime, newtime);
				break;

			default:
				return ignore_flat_deviation<10>(oldtime, newtime);
				break;
		}
	}

private:
	template<uint32_t MaxDiff>
	static std::optional<uint32_t> ignore_flat_deviation(uint32_t oldtime, uint32_t newtime) {
		uint32_t diff = std::abs((int32_t)oldtime - (int32_t)newtime);
		if (diff > MaxDiff)
			return newtime;
		else
			return std::nullopt;
	}

	static std::optional<uint32_t> ignore_percent_deviation(uint32_t oldtime, uint32_t newtime) {
		float dev = (float)newtime / (float)oldtime;
		if (dev > 1.01f || dev < 0.99f)
			return newtime;
		else
			return std::nullopt;
	}

	template<uint32_t MaxDiff>
	static uint32_t average_2_ignore_deviation(uint32_t oldtime, uint32_t newtime) {
		uint32_t diff = std::abs((int32_t)oldtime - (int32_t)newtime);
		if (diff > MaxDiff)
			return (oldtime + newtime) / 2;
		else
			return oldtime;
	}

	static uint32_t moving_average_4(uint32_t newtime) {
		ringbuff[ringbuff_pos] = newtime;

		// Use the clock period the first four times we receive ping after boot
		// After that, use an average of the previous 4 clock periods
		uint32_t ping_time = ringbuff_filled ? (ringbuff[0] + ringbuff[1] + ringbuff[2] + ringbuff[3]) / 4 : newtime;

		if (ringbuff_pos++ >= 4) {
			ringbuff_pos = 0;
			ringbuff_filled = true;
		}
		return ping_time;
	}

	static uint32_t moving_average_2(uint32_t oldtime, uint32_t newtime) {
		return (oldtime + newtime) / 2;
	}

	static uint32_t boxcar_average_4(uint32_t oldtime, uint32_t newtime) {
		ringbuff[ringbuff_pos] = newtime;

		if (++ringbuff_pos >= 4) {
			ringbuff_pos = 0;
			return (ringbuff[0] + ringbuff[1] + ringbuff[2] + ringbuff[3]) / 4;
		}
		return oldtime;
	}

	static uint32_t boxcar_average_8(uint32_t oldtime, uint32_t newtime) {
		ringbuff[ringbuff_pos] = newtime;

		if (++ringbuff_pos >= 8) {
			ringbuff_pos = 0;
			return (ringbuff[0] + ringbuff[1] + ringbuff[2] + ringbuff[3] + ringbuff[4] + ringbuff[5] + ringbuff[6] +
					ringbuff[7]) /
				   8;
		}
		return oldtime;
	}

	static uint32_t expo_average_4(uint32_t oldtime, uint32_t newtime) {
		uint32_t t = (float)oldtime * 0.75f + (float)newtime * 0.25f;
		return t & 0xFFFFFFF8;
	}

	static uint32_t expo_average_8(uint32_t oldtime, uint32_t newtime) {
		uint32_t t = (float)oldtime * 0.875f + (float)newtime * 0.125f;
		return t & 0xFFFFFFF8;
	}

	static inline uint32_t ringbuff[8]{};
	static inline uint32_t ringbuff_pos = 0;
	static inline bool ringbuff_filled = false;
};
