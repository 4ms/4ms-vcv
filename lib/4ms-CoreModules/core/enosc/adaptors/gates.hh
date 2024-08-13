#pragma once

#include "../easiglib/util.hh"
#include <cstdint>

namespace EnOsc
{

enum Gate {
	GATE_LEARN,
	GATE_FREEZE,
};

struct Gates : easiglib::Nocopy {

	struct GateJack {
		uint8_t state_;

		bool jack_state_;
		void set(bool s) {
			jack_state_ = s;
		}

		void Debounce() {
			state_ = (state_ << 1) | jack_state_;
		}
		bool just_disabled() const {
			return state_ == 0b01111111;
		}
		bool just_enabled() const {
			return state_ == 0b10000000;
		}
		bool enabled() const {
			return state_ == 0b00000000;
		}

	} learn_, freeze_;

	void Debounce() {
		learn_.Debounce();
		freeze_.Debounce();
	}
};

} // namespace EnOsc
