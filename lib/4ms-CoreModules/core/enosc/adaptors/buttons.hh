#pragma once

#include "../easiglib/util.hh"
#include <cstdint>

namespace EnOsc
{

enum Button {
	BUTTON_LEARN,
	BUTTON_FREEZE,
};

struct Buttons : easiglib::Nocopy {

	Buttons() {
		for (int i = 16; i--;) {
			Debounce();
		}
	}

	struct PushButton {
		uint8_t history_ = 0xFF;
		bool state_ = true;

		void Debounce() {
			history_ = (history_ << 1) | get();
		}
		bool just_released() const {
			return history_ == 0b01111111;
		}
		bool just_pushed() const {
			return history_ == 0b10000000;
		}
		bool pushed() const {
			return history_ == 0b00000000;
		}
		bool get() const {
			return state_;
		}
		void set(bool s) {
			state_ = !s;
		}
	} learn_, freeze_;

	void Debounce() {
		learn_.Debounce();
		freeze_.Debounce();
	}
};

} // namespace EnOsc
