#pragma once
#include "controls.hh"
#include "flags.hh"
#include "modes.hh"
#include "timer.hh"

namespace LDKit
{

struct TrigIns {
	Controls &controls;
	ChannelMode &modes;
	Flags &flags;
	Timer &timer;

	TrigIns(Controls &controls, ChannelMode &modes, Flags &flags, Timer &timer)
		: controls{controls}
		, modes{modes}
		, flags{flags}
		, timer{timer} {}

	uint32_t update_ping(uint32_t ping_time) {
		if (controls.ping_jack.just_went_high()) {
			uint32_t t_ping_tmr = timer.get_ping_tmr();
			timer.reset_ping_tmr();
			int32_t diff = std::abs((int32_t)ping_time - (int32_t)t_ping_tmr);
			if (diff > 10) {
				controls.clk_out.high();
				timer.reset_clkout_tmr();
				controls.ping_led.high();
				timer.reset_pingled_tmr();

				if (!modes.ping_locked)
					flags.set_time_changed();

				ping_time = t_ping_tmr;
			}
		}

		return ping_time;
	}
};

} // namespace LDKit
