#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct GateInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Gate"};
    static constexpr std::string_view description{"Gate Delay"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/Gate_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Gate.png"};

    using enum Coords;

    static constexpr std::array<Element, 6> Elements{{
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(46.85), Center, "Length", ""}, 0.25f},
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(94.99), Center, "Delay", ""}, 0.0f},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(168.72), Center, "Length CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(216.89), Center, "Delay CV", ""}},
		GateJackInput4ms{{to_mm<72>(28.8), to_mm<72>(265.06), Center, "Input", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(313.23), Center, "Out", ""}},
}};

    enum class Elem {
        LengthKnob,
        DelayKnob,
        LengthCvIn,
        DelayCvIn,
        InputIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobLength, 
        KnobDelay, 
        NumKnobs,
    };
    
    
    enum {
        InputLength_Cv, 
        InputDelay_Cv, 
        InputInput, 
        NumInJacks,
    };
    
    enum {
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
