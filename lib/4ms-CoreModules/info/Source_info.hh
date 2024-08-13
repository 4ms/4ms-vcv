#pragma once
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct SourceInfo : ModuleInfoBase {
	static constexpr std::string_view slug{"Source"};
	static constexpr std::string_view description{"Source"};
	static constexpr uint32_t width_hp = 4;
	static constexpr std::string_view svg_filename{"res/modules/Source_artwork.svg"};
	static constexpr std::string_view png_filename{"4ms/fp/Source.png"};

	using enum Coords;

	static constexpr std::array<Element, 4> Elements{{
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(46.72), Center, "1", ""}},
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(94.96), Center, "2", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(265.11), Center, " 1 Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(313.21), Center, " 2 Out", ""}},
	}};

	enum class Elem {
		_1Knob,
		_2Knob,
		_1Out,
		_2Out,
	};

	// Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)

	enum {
		Knob_1,
		Knob_2,
		NumKnobs,
	};

	enum {
		Output_1_Out,
		Output_2_Out,
		NumOutJacks,
	};
};
} // namespace MetaModule
