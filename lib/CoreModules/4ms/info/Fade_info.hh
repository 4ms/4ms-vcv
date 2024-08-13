#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct FadeInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Fade"};
    static constexpr std::string_view description{"Crossfader"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/Fade_artwork.svg"};
	static constexpr std::string_view png_filename{"4ms/fp/Fade.png"};

    using enum Coords;

    static constexpr std::array<Element, 5> Elements{{
		Knob9mm{{to_mm<72>(28.92), to_mm<72>(46.99), Center, "Fade", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.93), to_mm<72>(168.85), Center, "CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.93), to_mm<72>(216.11), Center, "In 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.93), to_mm<72>(263.68), Center, "In 2", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.93), to_mm<72>(312.29), Center, "Out", ""}},
}};

    enum class Elem {
        FadeKnob,
        CvIn,
        In1In,
        In2In,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobFade = 0,
        NumKnobs,
    };
    
    
    enum {
        InputCv = 0,
        InputIn_1 = 1,
        InputIn_2 = 2,
        NumInJacks,
    };
    
    enum {
        OutputOut = 0,
        NumOutJacks,
    };
    
};
} // namespace MetaModule
