#include "util/colors.hh"
#include <cstdint>
#include <rack.hpp>

namespace MetaModule
{

inline uint16_t cable_color_rgb565(rack::app::CableWidget *cableWidget) {
	if (!cableWidget)
		return 0;

	auto r_amt = (uint8_t)(std::clamp(cableWidget->color.r, 0.f, 1.f) * 255);
	auto g_amt = (uint8_t)(std::clamp(cableWidget->color.g, 0.f, 1.f) * 255);
	auto b_amt = (uint8_t)(std::clamp(cableWidget->color.b, 0.f, 1.f) * 255);
	return Color(r_amt, g_amt, b_amt).Rgb565();
}

} // namespace MetaModule
