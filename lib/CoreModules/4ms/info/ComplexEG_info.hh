#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct ComplexEGInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"ComplexEG"};
    static constexpr std::string_view description{"Complex EnvelopeGenerator"};
    static constexpr uint32_t width_hp = 15;
    static constexpr std::string_view svg_filename{"res/modules/ComplexEG_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/ComplexEG.png"};

    using enum Coords;

    static constexpr std::array<Element, 21> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(43.5), to_mm<72>(46.07), Center, "Attack", ""}, 0.125f},
		Davies1900hBlackKnob{{to_mm<72>(108.0), to_mm<72>(45.4), Center, "Decay", ""}, 0.125f},
		Davies1900hBlackKnob{{to_mm<72>(172.5), to_mm<72>(45.4), Center, "Release", ""}, 0.25f},
		Davies1900hBlackKnob{{to_mm<72>(43.5), to_mm<72>(104.39), Center, "A Curve", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(108.0), to_mm<72>(104.39), Center, "D Curve", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(172.5), to_mm<72>(104.39), Center, "R Curve", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(43.5), to_mm<72>(163.16), Center, "Sustain", ""}, 0.75f},
		OrangeButton{{to_mm<72>(108.0), to_mm<72>(159.65), Center, "Loop", ""}},
		Davies1900hBlackKnob{{to_mm<72>(172.39), to_mm<72>(163.1), Center, "Hold", ""}, 0.5f},
		AnalogJackInput4ms{{to_mm<72>(36.32), to_mm<72>(214.01), Center, "Gate In", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.84), to_mm<72>(214.01), Center, "Attack CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(131.84), to_mm<72>(214.01), Center, "Decay CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(179.84), to_mm<72>(214.01), Center, "Release CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(36.32), to_mm<72>(265.59), Center, "Attack Out", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.84), to_mm<72>(265.59), Center, "Sustain CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(131.84), to_mm<72>(265.59), Center, "Hold CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(179.84), to_mm<72>(265.59), Center, "Decay Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(36.32), to_mm<72>(309.72), Center, "Release Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.84), to_mm<72>(309.72), Center, "Sustain Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(131.84), to_mm<72>(309.72), Center, "Hold Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(179.84), to_mm<72>(309.72), Center, "Out", ""}},
}};

    enum class Elem {
        AttackKnob,
        DecayKnob,
        ReleaseKnob,
        ACurveKnob,
        DCurveKnob,
        RCurveKnob,
        SustainKnob,
        LoopButton,
        HoldKnob,
        GateIn,
        AttackCvIn,
        DecayCvIn,
        ReleaseCvIn,
        AttackOut,
        SustainCvIn,
        HoldCvIn,
        DecayOut,
        ReleaseOut,
        SustainOut,
        HoldOut,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobAttack, 
        KnobDecay, 
        KnobRelease, 
        KnobA_Curve, 
        KnobD_Curve, 
        KnobR_Curve, 
        KnobSustain, 
        KnobHold, 
        NumKnobs,
    };
    
    enum {
        SwitchLoop, 
        NumSwitches,
    };
    
    enum {
        InputGate_In, 
        InputAttack_Cv, 
        InputDecay_Cv, 
        InputRelease_Cv, 
        InputSustain_Cv, 
        InputHold_Cv, 
        NumInJacks,
    };
    
    enum {
        OutputAttack_Out, 
        OutputDecay_Out, 
        OutputRelease_Out, 
        OutputSustain_Out, 
        OutputHold_Out, 
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
