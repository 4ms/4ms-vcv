#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct EnOscInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"EnOsc"};
    static constexpr std::string_view description{"Ensemble Oscillator"};
    static constexpr uint32_t width_hp = 16;
    static constexpr std::string_view svg_filename{"res/modules/EnOsc_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/EnOsc.png"};

    using enum Coords;

    static constexpr std::array<Element, 32> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(54.1), to_mm<72>(78.08), Center, "Scale", ""}, 0.453125f},
		Davies1900hBlackKnob{{to_mm<72>(115.27), to_mm<72>(61.62), Center, "Spread", ""}, 0.375f},
		Davies1900hBlackKnob{{to_mm<72>(176.96), to_mm<72>(78.07), Center, "Pitch", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(32.72), to_mm<72>(144.14), Center, "Balance", ""}, 0.5f},
		DaviesLargeKnob{{to_mm<72>(115.56), to_mm<72>(140.93), Center, "Root", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(198.23), to_mm<72>(144.14), Center, "Cross FM", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(61.77), to_mm<72>(208.21), Center, "Twist", ""}, 0.0f},
		Knob9mm{{to_mm<72>(115.49), to_mm<72>(207.86), Center, "Detune", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(168.87), to_mm<72>(208.21), Center, "Warp", ""}, 0.0f},
		Toggle3pos{{to_mm<72>(16.93), to_mm<72>(103.025), Center, "Scale Switch", ""}, {"Free", "Oct.", "12TET"}, Toggle3pos::State_t::UP},
		Toggle3pos{{to_mm<72>(214.23), to_mm<72>(102.945), Center, "Cross FM Switch", ""}, {"Down", "All", "Up"}, Toggle3pos::State_t::UP},
		Toggle3pos{{to_mm<72>(16.93), to_mm<72>(215.065), Center, "Twist Switch", ""}, {"Crush", "Pulsar", "Ramp"}},
		Toggle3pos{{to_mm<72>(214.23), to_mm<72>(201.725), Center, "Warp Switch", ""}, {"Segment", "Cheb", "Fold"}, Toggle3pos::State_t::UP},
		MomentaryRGB7mm{{to_mm<72>(21.63), to_mm<72>(42.53), Center, "Learn", ""}},
		MomentaryRGB7mm{{to_mm<72>(209.56), to_mm<72>(42.53), Center, "Freeze", ""}},
		AnalogJackInput4ms{{to_mm<72>(21.88), to_mm<72>(262.78), Center, "Pitch Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(21.88), to_mm<72>(305.91), Center, "Root Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(59.42), to_mm<72>(275.86), Center, "Scale Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(96.88), to_mm<72>(262.78), Center, "Spread Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(134.28), to_mm<72>(262.78), Center, "Cross FM Jack", ""}},
		GateJackInput4ms{{to_mm<72>(171.75), to_mm<72>(275.86), Center, "Learn Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(59.42), to_mm<72>(319.0), Center, "Balance Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(96.88), to_mm<72>(305.93), Center, "Twist Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(134.28), to_mm<72>(305.93), Center, "Warp Jack", ""}},
		GateJackInput4ms{{to_mm<72>(171.75), to_mm<72>(319.03), Center, "Freeze Jack", ""}},
		AnalogJackOutput4ms{{to_mm<72>(209.12), to_mm<72>(262.78), Center, "Out A", ""}},
		AnalogJackOutput4ms{{to_mm<72>(209.12), to_mm<72>(305.91), Center, "Out B", ""}},
		AltParamChoiceLabeled{{{to_mm<72>(61.77), to_mm<72>(208.21), Center, "FreezeSplit", ""}, 3, 1}, {"Even/Odd", "Low/High", "Root/Others"}},
		AltParamChoiceLabeled{{{to_mm<72>(168.87), to_mm<72>(208.21), Center, "StereoSplit", ""}, 3, 1}, {"Even/Odd", "Low/High", "Root/Others"}},
		AltParamContinuous{{to_mm<72>(32.72), to_mm<72>(144.14), Center, "Crossfade", ""}, 0.5f},
		AltParamChoice{{to_mm<72>(115.27), to_mm<72>(61.62), Center, "NumOsc", ""}, 16, 16},
		AltParamContinuous{{to_mm<72>(176.96), to_mm<72>(78.07), Center, "FineTune", ""}, 0.5f},
}};

    enum class Elem {
        ScaleKnob,
        SpreadKnob,
        PitchKnob,
        BalanceKnob,
        RootKnob,
        CrossFmKnob,
        TwistKnob,
        DetuneKnob,
        WarpKnob,
        ScaleSwitch,
        CrossFmSwitch,
        TwistSwitch,
        WarpSwitch,
        LearnButton,
        FreezeButton,
        PitchJackIn,
        RootJackIn,
        ScaleJackIn,
        SpreadJackIn,
        CrossFmJackIn,
        LearnJackIn,
        BalanceJackIn,
        TwistJackIn,
        WarpJackIn,
        FreezeJackIn,
        OutAOut,
        OutBOut,
        FreezesplitAltParam,
        StereosplitAltParam,
        CrossfadeAltParam,
        NumoscAltParam,
        FinetuneAltParam,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobScale, 
        KnobSpread, 
        KnobPitch, 
        KnobBalance, 
        KnobRoot, 
        KnobCross_Fm, 
        KnobTwist, 
        KnobDetune, 
        KnobWarp, 
        NumKnobs,
    };
    
    enum {
        SwitchScale_Switch, 
        SwitchCross_Fm_Switch, 
        SwitchTwist_Switch, 
        SwitchWarp_Switch, 
        SwitchLearn, 
        SwitchFreeze, 
        NumSwitches,
    };
    
    enum {
        InputPitch_Jack, 
        InputRoot_Jack, 
        InputScale_Jack, 
        InputSpread_Jack, 
        InputCross_Fm_Jack, 
        InputLearn_Jack, 
        InputBalance_Jack, 
        InputTwist_Jack, 
        InputWarp_Jack, 
        InputFreeze_Jack, 
        NumInJacks,
    };
    
    enum {
        OutputOut_A, 
        OutputOut_B, 
        NumOutJacks,
    };
    
    
    enum {
        AltParamFreezesplit, 
        AltParamStereosplit, 
        AltParamCrossfade, 
        AltParamNumosc, 
        AltParamFinetune, 
    };
};
} // namespace MetaModule
