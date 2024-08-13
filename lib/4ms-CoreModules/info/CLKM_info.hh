#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct CLKMInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"CLKM"};
    static constexpr std::string_view description{"Clock Multiplier"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/CLKM_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/CLKM.png"};

    using enum Coords;

    static constexpr std::array<Element, 4> Elements{{
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(46.77), Center, "Multiply", ""}, 0.0f},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(216.85), Center, "CV", ""}},
		GateJackInput4ms{{to_mm<72>(28.8), to_mm<72>(265.04), Center, "Clk In", ""}},
		GateJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(313.23), Center, "Clk Out", ""}},
}};

    enum class Elem {
        MultiplyKnob,
        CvIn,
        ClkIn,
        ClkOut,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobMultiply, 
        NumKnobs,
    };
    
    
    enum {
        InputCv, 
        InputClk_In, 
        NumInJacks,
    };
    
    enum {
        OutputClk_Out, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
