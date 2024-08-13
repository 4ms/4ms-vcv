#pragma once
#include <cstdint>

struct CrossfadingReadHead {
	uint32_t cur;
	uint32_t fade_ending_addr;
	uint32_t queued_ending_addr;
	uint32_t queued_divmult;
	float phase;
	bool is_reverse_fading;

	void increment(float rate) {
		phase += rate;
		if (phase > 1.f) {
			phase = 0.f;
			is_reverse_fading = false;
			cur = fade_ending_addr;
			if (queued_divmult) {
				// set new?
			} else if (queued_ending_addr) {
			}
		}
	}

	bool is_crossfading() { return phase > 0.f; }
};
