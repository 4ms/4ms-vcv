#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct MultiLFOInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"MultiLFO"};
    static constexpr std::string_view description{"Multi LFO"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/MultiLFO_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/MultiLFO.png"};

    using enum Coords;

    static constexpr std::array<Element, 11> Elements{{
		DaviesLargeKnob{{to_mm<72>(57.67), to_mm<72>(55.73), Center, "Rate", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(31.96), to_mm<72>(119.41), Center, "Phase", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(83.49), to_mm<72>(119.75), Center, "PW", ""}, 0.5f},
		AnalogJackInput4ms{{to_mm<72>(32.1), to_mm<72>(167.36), Center, "Reset", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.63), to_mm<72>(167.36), Center, "PW CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(32.1), to_mm<72>(214.57), Center, "Rate CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.63), to_mm<72>(214.57), Center, "Phase CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(32.1), to_mm<72>(263.64), Center, "Inv Saw", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.63), to_mm<72>(263.67), Center, "Pulse", ""}},
		AnalogJackOutput4ms{{to_mm<72>(32.1), to_mm<72>(311.23), Center, "Saw", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.63), to_mm<72>(311.26), Center, "Sine", ""}},
}};

    enum class Elem {
        RateKnob,
        PhaseKnob,
        PwKnob,
        ResetIn,
        PwCvIn,
        RateCvIn,
        PhaseCvIn,
        InvSawOut,
        PulseOut,
        SawOut,
        SineOut,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobRate, 
        KnobPhase, 
        KnobPw, 
        NumKnobs,
    };
    
    
    enum {
        InputReset, 
        InputPw_Cv, 
        InputRate_Cv, 
        InputPhase_Cv, 
        NumInJacks,
    };
    
    enum {
        OutputInv_Saw, 
        OutputPulse, 
        OutputSaw, 
        OutputSine, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
