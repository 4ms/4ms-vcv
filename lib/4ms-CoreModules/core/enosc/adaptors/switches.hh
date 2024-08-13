#pragma once

#include "../easiglib/util.hh"

namespace EnOsc
{

struct Switches : easiglib::Nocopy {

	enum Switch { SCALE, MOD, TWIST, WARP };
	enum State { UP = 1, DOWN = 2, MID = 3 };

	struct ThreePosSwitch {
		State next_state_ = MID;
		State state_ = MID;
		State last_state_ = MID;

		void Debounce() {
			last_state_ = state_;
			state_ = next_state_;
		}

		// Always called after Switches::Debounce():
		bool just_switched_mid() {
			return state_ == MID && last_state_ != MID;
		}
		bool just_switched_up() {
			return state_ == UP && last_state_ != UP;
		}
		bool just_switched_down() {
			return state_ == DOWN && last_state_ != DOWN;
		}

		State get() {
			return state_;
		}

		void set(State s) {
			next_state_ = s;
		}
	};

	struct Scale : ThreePosSwitch {
	} scale_;

	struct Mod : ThreePosSwitch {
	} mod_;

	struct Twist : ThreePosSwitch {
	} twist_;

	struct Warp : ThreePosSwitch {
	} warp_;
};

} // namespace EnOsc
