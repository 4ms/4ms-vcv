#pragma once

#include <cstdint>
#include <limits>

namespace MetaModule::PEG::Mocks
{

struct InputPin
{
    void update()
    {
    }
	
    bool just_went_high()
    {
		auto result = value and not compareValue;
		if (result)
		{
			compareValue = value;
		}
		return result;
    }

	bool just_went_low()
    {
		auto result = not value and compareValue;
		if (result)
		{
			compareValue = value;
		}
		return result;
    }

	void sideload_set(bool val)
	{
		value = val;
	}

protected:
	bool value = false;
	bool compareValue = false;
};


struct MockedTrigger : public InputPin
{
	bool is_pressed() { return value; }
	bool is_just_released()  { return just_went_low(); }
    bool is_just_pressed()  { return just_went_high(); }


};

struct MockedButton : public MockedTrigger
{
	void sideload_set(bool newVal)
	{
		if (newVal != value)
		{
			steady_state_ctr = 0;
		}
		else
		{
			if (steady_state_ctr < std::numeric_limits<uint32_t>::max() - 1) steady_state_ctr++;
		}

		value = newVal;
	}
	unsigned how_long_held() { return steady_state_ctr; }
	unsigned how_long_held_pressed() { return is_pressed() ? steady_state_ctr : 0; }

private:
	uint32_t steady_state_ctr = 0;
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
	SwitchPos state = SwitchPos(0);
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
	bool value = false;
};

struct MockedLED : OutputPin
{
};


}
