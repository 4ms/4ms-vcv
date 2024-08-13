#pragma once

#include <array>

#include <rack.hpp>

struct PaletteHub {
	static inline const NVGcolor ORANGE = nvgRGB(0xff, 0x80, 0x00);
	static inline const NVGcolor BROWN = nvgRGB(0x80, 0x40, 0x00);
	static inline const NVGcolor BLACK = nvgRGB(0x00, 0x00, 0x00);
	static inline const NVGcolor RED = nvgRGB(0xff, 0x00, 0x00);
	static inline const NVGcolor PURPLE = nvgRGB(0x80, 0x00, 0x40);
	static inline const NVGcolor GREEN = nvgRGB(0x00, 0xff, 0x00);
	static inline const NVGcolor BLUE = nvgRGB(0x00, 0x00, 0xff);
	static inline const NVGcolor CYAN = nvgRGB(0x00, 0xff, 0xff);
	static inline const NVGcolor MAGENTA = nvgRGB(0xff, 0x00, 0xff);
	static inline const NVGcolor YELLOW = nvgRGB(0xff, 0xff, 0x00);
	static inline const NVGcolor GRAY50 = nvgRGB(0x7f, 0x7f, 0x7f);
	static inline const NVGcolor GRAY25 = nvgRGB(0x40, 0x40, 0x40);
	static inline const NVGcolor GRAY75 = nvgRGB(0xc0, 0xc0, 0xc0);
	static inline const NVGcolor WHITE = nvgRGB(0xff, 0xff, 0xff);

private:
	static inline const std::array<NVGcolor, 12> _color = {
		RED,
		YELLOW,
		CYAN,
		MAGENTA,
		ORANGE,
		GREEN,
		//TODO: less saturated for the knobs u-z?
		RED,
		YELLOW,
		CYAN,
		MAGENTA,
		ORANGE,
		GREEN,
	};

public:
	static NVGcolor color(unsigned idx) {
		return _color[idx % _color.size()];
	}
};
