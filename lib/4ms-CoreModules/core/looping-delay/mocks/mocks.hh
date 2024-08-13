#pragma once
#include <limits>
#include <cstdint>
#include "util/debouncer.hh"

namespace LDKit::Mocks
{


struct MockedTrigger : public Debouncer<0b0001, 0b1110, 0b1111>
{
	void update() {}
};


struct MockedButton : public DebouncerCounter<0b01, 0b10, 0b11>
{
	void update() {}

	void sideload_set(bool newVal) {
		register_state(newVal);
	}

	unsigned how_long_held() {
		return steady_state_ctr;
	}

	unsigned how_long_held_pressed() {
		return is_pressed() ? steady_state_ctr : 0;
	}

	void reset_hold_ctr() {
		steady_state_ctr = 0;
	}
};



enum class SwitchPos { Invalid = 0b00, Up = 0b01, Down = 0b10, Center = 0b11 };

struct MockedSwitch3Pos
{
	MockedSwitch3Pos() : state(SwitchPos::Down) {}
	SwitchPos read()
	{
		return state;
	}

	void sideload_set(SwitchPos val)
	{
		state = val;
	}

private:
	SwitchPos state;
};

struct OutputPin
{
	void set(bool val)
	{
		if (val) high();
		else low();
	}

	void low()
	{
		value = false;
	}

	void high()
	{
		value = true;
	}

	bool sideload_get()
	{
		return value;
	}

private:
	bool value;
};

struct MockedLED : OutputPin
{
};


}
