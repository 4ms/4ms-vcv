#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct VerbInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Verb"};
    static constexpr std::string_view description{"Reverb Effect"};
    static constexpr uint32_t width_hp = 10;
    static constexpr std::string_view svg_filename{"res/modules/Verb_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Verb.png"};

    using enum Coords;

    static constexpr std::array<Element, 15> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(37.73), to_mm<72>(46.3), Center, "Size", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(106.27), to_mm<72>(46.3), Center, "Time", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(37.73), to_mm<72>(104.5), Center, "Damp", ""}, 0.25f},
		Davies1900hBlackKnob{{to_mm<72>(106.27), to_mm<72>(104.5), Center, "AP Ratio", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(37.73), to_mm<72>(162.7), Center, "Comb", ""}, 0.25f},
		Davies1900hBlackKnob{{to_mm<72>(106.27), to_mm<72>(162.7), Center, "Mix", ""}, 0.875f},
		AnalogJackInput4ms{{to_mm<72>(29.82), to_mm<72>(231.98), Center, "Input", ""}},
		AnalogJackInput4ms{{to_mm<72>(72.0), to_mm<72>(231.98), Center, "Size CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(115.04), to_mm<72>(231.98), Center, "Time CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(29.82), to_mm<72>(272.11), Center, "Damp CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(72.0), to_mm<72>(272.11), Center, "Ratio CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(115.04), to_mm<72>(272.11), Center, "Comb CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(29.82), to_mm<72>(313.71), Center, "Mix CV", ""}},
		GateJackInput4ms{{to_mm<72>(72.0), to_mm<72>(313.71), Center, "Hold Gate", ""}},
		AnalogJackOutput4ms{{to_mm<72>(115.04), to_mm<72>(313.71), Center, "Out", ""}},
}};

    enum class Elem {
        SizeKnob,
        TimeKnob,
        DampKnob,
        ApRatioKnob,
        CombKnob,
        MixKnob,
        InputIn,
        SizeCvIn,
        TimeCvIn,
        DampCvIn,
        RatioCvIn,
        CombCvIn,
        MixCvIn,
        HoldGateIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobSize, 
        KnobTime, 
        KnobDamp, 
        KnobAp_Ratio, 
        KnobComb, 
        KnobMix, 
        NumKnobs,
    };
    
    
    enum {
        InputInput, 
        InputSize_Cv, 
        InputTime_Cv, 
        InputDamp_Cv, 
        InputRatio_Cv, 
        InputComb_Cv, 
        InputMix_Cv, 
        InputHold_Gate, 
        NumInJacks,
    };
    
    enum {
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
