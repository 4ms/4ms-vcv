/*
 * leds.h
 */

#pragma once

#include "pwm.h"
#include <stdint.h>

namespace MetaModule::PEG
{

enum RgbLeds {
	LED_PING,
	LED_CYCLE,
	LED_ENVA,
	LED_ENVB,

	NUM_RGB_LEDS
};

enum MonoLeds {
	LED_TRIGOUT,

	NUM_MONO_LEDS
};

struct Color {
	uint16_t r = 0;
	uint16_t g = 0;
	uint16_t b = 0;
};

struct AdjustedColor {
	Color ping;
	Color cycle;
	Color envA;
	Color envB;
};

enum Palette {
	c_OFF,
	c_GREY50,
	c_WHITE,
	c_FULLWHITE,
	c_RED,
	c_ORANGE,
	c_YELLOW,
	c_GREEN,
	c_CYAN,
	c_BLUE,
	c_DIMBLUE,
	c_PURPLE,

	NUM_COLORS
};

}