#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct KPLSInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"KPLS"};
    static constexpr std::string_view description{"Karplus Strong Voice"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/KPLS_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/KPLS.png"};

    using enum Coords;

    static constexpr std::array<Element, 6> Elements{{
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(46.77), Center, "Pitch", ""}, 0.5f},
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(94.96), Center, "Decay", ""}, 0.25f},
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(143.15), Center, "Spread", ""}, 0.5f},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(216.85), Center, "V/Oct", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(265.04), Center, "Trig", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(313.26), Center, "Out", ""}},
}};

    enum class Elem {
        PitchKnob,
        DecayKnob,
        SpreadKnob,
        V_OctIn,
        TrigIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobPitch, 
        KnobDecay, 
        KnobSpread, 
        NumKnobs,
    };
    
    
    enum {
        InputV_Oct, 
        InputTrig, 
        NumInJacks,
    };
    
    enum {
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
