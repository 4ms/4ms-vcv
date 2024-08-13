#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct Atvert2Info : ModuleInfoBase {
    static constexpr std::string_view slug{"Atvert2"};
    static constexpr std::string_view description{"Dual Attenuverter"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/Atvert2_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Atvert2.png"};

    using enum Coords;

    static constexpr std::array<Element, 6> Elements{{
		Knob9mm{{to_mm<72>(28.9), to_mm<72>(46.86), Center, "1", ""}, 1.0f},
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(94.96), Center, "2", ""}, 1.0f},
		AnalogJackInput4ms{{to_mm<72>(28.88), to_mm<72>(168.72), Center, "In 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.88), to_mm<72>(216.87), Center, "In 2", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.88), to_mm<72>(265.02), Center, "Out 1", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.88), to_mm<72>(313.16), Center, "Out 2", ""}},
}};

    enum class Elem {
        _1Knob,
        _2Knob,
        In1In,
        In2In,
        Out1Out,
        Out2Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        Knob_1, 
        Knob_2, 
        NumKnobs,
    };
    
    
    enum {
        InputIn_1, 
        InputIn_2, 
        NumInJacks,
    };
    
    enum {
        OutputOut_1, 
        OutputOut_2, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
