#pragma once
#include "../mocks/mocks.hh"
#include "ping_methods.hh"
#include <cstdint>

namespace LDKit
{

class Timer {
public:
	Mocks::MockedTrigger ping_jack;
	Mocks::OutputPin clk_out;
	Mocks::OutputPin bus_clk_out;
	Mocks::MockedLED loop_led;

	Mocks::OutputPin loop_out;

private:
	uint32_t _ping_tmr = 0;
	uint32_t _ping_time = 12000;
	bool _ping_changed = true;
	uint32_t _pingled_tmr = 0;
	float _loop_tmr = 0;
	float _loop_time = 0;
	bool _ping_led_high = false;
	bool _ping_cycled = false;
	bool _ping_tmr_needs_reset = false;
	PingMethod &_ping_method;

public:
	Timer(PingMethod &ping_method)
		: _ping_method{ping_method} {
	}

	void start() {
		// do nothing
	}

	// TODO: call this with every sample/block
	void inc() {
		if (_ping_tmr_needs_reset) {
			_ping_tmr_needs_reset = false;
			_ping_tmr = 0;
			clk_out.high();
			bus_clk_out.high();
		} else
			_ping_tmr++;

		_pingled_tmr++;
		_loop_tmr++;

		ping_jack.update();
		if (ping_jack.just_went_high()) {
			// TODO: if ping_method != last_ping_method PingMethodAlgorithm::reset();
			auto newtime = PingMethodAlgorithm::filter(_ping_time, _ping_tmr, _ping_method);
			if (newtime.has_value()) {
				_ping_time = newtime.value();
				_pingled_tmr = 0;

				_ping_changed = true;
				clk_out.high();
				bus_clk_out.high();
			}
			_ping_tmr = 0;
		}

		if (_pingled_tmr >= _ping_time) {
			_pingled_tmr = 0;
			_ping_led_high = true;
			_ping_cycled = true;
			clk_out.high();
			bus_clk_out.high();
		} else if (_pingled_tmr >= (_ping_time / 2)) {
			_ping_led_high = false;
			clk_out.low();
			bus_clk_out.low();
		}

		if (_loop_tmr >= _loop_time) {
			reset_loop_tmr(_loop_tmr - _loop_time);
		} else if (_loop_tmr >= (_loop_time / 2)) {
			loop_led.low();
			loop_out.low();
		}
	}

	void set_divmult_time(float time) {
		_loop_time = time;
	}

	bool take_ping_changed() {
		if (_ping_changed) {
			_ping_changed = false;
			return true;
		}
		return false;
	}

	uint32_t get_ping_time() {
		return _ping_time;
	}

	uint32_t reset_ping_tmr() {
		_ping_time = _ping_tmr;
		_ping_tmr_needs_reset = true;
		return _ping_time;
	}

	void reset_loop_tmr(float reset_to = 0.f) {
		loop_led.high();
		loop_out.high();
		_loop_tmr = reset_to;
	}

	bool ping_led_high() {
		return _ping_led_high;
	}

	bool take_ping_cycled() {
		auto t = _ping_cycled;
		_ping_cycled = false;
		return t;
	}

	void scale_periods(float factor) {
		_ping_time = (float)_ping_time * factor;
		_ping_tmr = (float)_ping_tmr * factor;
		_pingled_tmr = (float)_pingled_tmr * factor;
		_loop_time = (float)_loop_time * factor;
		_loop_tmr = (float)_loop_tmr * factor;
		_ping_changed = true;
	}

	void set_ping_time(uint32_t time) {
		_ping_tmr = time;
		_pingled_tmr = 0;
		reset_ping_tmr();
		reset_loop_tmr();
		_ping_changed = true;
	}
};

} // namespace LDKit
