#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct DetuneInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Detune"};
    static constexpr std::string_view description{"Detuner"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/Detune_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Detune.png"};

    using enum Coords;

    static constexpr std::array<Element, 8> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(31.82), to_mm<72>(57.84), Center, "W Speed", ""}, 0.25f},
		Davies1900hBlackKnob{{to_mm<72>(83.35), to_mm<72>(57.84), Center, "F Speed", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(31.82), to_mm<72>(119.08), Center, "W Depth", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(83.35), to_mm<72>(119.08), Center, "F Depth", ""}, 0.5f},
		AnalogJackInput4ms{{to_mm<72>(31.99), to_mm<72>(271.96), Center, "Input", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.18), to_mm<72>(271.96), Center, "Detune", ""}},
		AnalogJackInput4ms{{to_mm<72>(31.99), to_mm<72>(313.57), Center, "Depth", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.18), to_mm<72>(313.57), Center, "Out", ""}},
}};

    enum class Elem {
        WSpeedKnob,
        FSpeedKnob,
        WDepthKnob,
        FDepthKnob,
        InputIn,
        DetuneIn,
        DepthIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobW_Speed, 
        KnobF_Speed, 
        KnobW_Depth, 
        KnobF_Depth, 
        NumKnobs,
    };
    
    
    enum {
        InputInput, 
        InputDetune, 
        InputDepth, 
        NumInJacks,
    };
    
    enum {
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
