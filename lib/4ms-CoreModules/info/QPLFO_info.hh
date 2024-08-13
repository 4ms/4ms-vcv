#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct QPLFOInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"QPLFO"};
    static constexpr std::string_view description{"Quad Pingable LFO"};
    static constexpr uint32_t width_hp = 12;
    static constexpr std::string_view svg_filename{"res/modules/QPLFO_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/QPLFO.png"};

    using enum Coords;

    static constexpr std::array<Element, 37> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(114.03), to_mm<72>(59.68), Center, "Skew 1", ""}, 0.5f},
		WhiteMomentary7mm{{to_mm<72>(154.79), to_mm<72>(43.18), Center, "Ping 1", ""}},
		OrangeButton{{to_mm<72>(154.79), to_mm<72>(81.06), Center, "On 1", ""}},
		Davies1900hBlackKnob{{to_mm<72>(114.03), to_mm<72>(135.28), Center, "Skew 2", ""}, 0.5f},
		WhiteMomentary7mm{{to_mm<72>(154.79), to_mm<72>(118.78), Center, "Ping 2", ""}},
		OrangeButton{{to_mm<72>(154.79), to_mm<72>(156.66), Center, "On 2", ""}},
		Davies1900hBlackKnob{{to_mm<72>(114.03), to_mm<72>(210.88), Center, "Skew 3", ""}, 0.5f},
		WhiteMomentary7mm{{to_mm<72>(154.79), to_mm<72>(194.38), Center, "Ping 3", ""}},
		OrangeButton{{to_mm<72>(154.79), to_mm<72>(232.26), Center, "On 3", ""}},
		Davies1900hBlackKnob{{to_mm<72>(114.03), to_mm<72>(286.48), Center, "Skew 4", ""}, 0.5f},
		WhiteMomentary7mm{{to_mm<72>(154.79), to_mm<72>(269.98), Center, "Ping 4", ""}},
		OrangeButton{{to_mm<72>(154.79), to_mm<72>(307.86), Center, "On 4", ""}},
		GateJackInput4ms{{to_mm<72>(22.57), to_mm<72>(42.18), Center, "Ping 1 Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(67.94), to_mm<72>(59.46), Center, "Skew 1 CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(22.57), to_mm<72>(76.74), Center, "Out 1", ""}},
		GateJackInput4ms{{to_mm<72>(67.94), to_mm<72>(94.02), Center, "Reset 1", ""}},
		GateJackInput4ms{{to_mm<72>(22.57), to_mm<72>(117.78), Center, "Ping 2 Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(67.94), to_mm<72>(135.06), Center, "Skew 2 Jack", ""}},
		AnalogJackOutput4ms{{to_mm<72>(22.57), to_mm<72>(152.34), Center, "Out 2", ""}},
		GateJackInput4ms{{to_mm<72>(67.94), to_mm<72>(169.62), Center, "Reset 2", ""}},
		GateJackInput4ms{{to_mm<72>(22.57), to_mm<72>(193.38), Center, "Ping 3 Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(67.94), to_mm<72>(210.66), Center, "Skew 3 Jack", ""}},
		AnalogJackOutput4ms{{to_mm<72>(22.57), to_mm<72>(227.94), Center, "Out 3", ""}},
		GateJackInput4ms{{to_mm<72>(67.94), to_mm<72>(245.22), Center, "Reset 3", ""}},
		GateJackInput4ms{{to_mm<72>(22.57), to_mm<72>(268.98), Center, "Ping 4 Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(67.94), to_mm<72>(286.26), Center, "Skew 4 Jack", ""}},
		AnalogJackOutput4ms{{to_mm<72>(22.57), to_mm<72>(303.54), Center, "Out 4", ""}},
		GateJackInput4ms{{to_mm<72>(67.94), to_mm<72>(320.82), Center, "Reset 4", ""}},
		RedLight{{to_mm<72>(45.23), to_mm<72>(76.95), Center, "LED 1", ""}},
		WhiteLight{{to_mm<72>(45.23), to_mm<72>(152.55), Center, "LED 2", ""}},
		BlueLight{{to_mm<72>(45.23), to_mm<72>(228.15), Center, "LED 3", ""}},
		GreenLight{{to_mm<72>(45.23), to_mm<72>(303.75), Center, "LED 4", ""}},
		AltParamChoiceLabeled{{{to_mm<72>(154.79), to_mm<72>(43.18), Center, "Fire On Unmute ch1", ""}, 2, 1}, {"off", "on"}},
		AltParamChoiceLabeled{{{to_mm<72>(154.79), to_mm<72>(118.78), Center, "Fire On Unmute ch2", ""}, 2, 1}, {"off", "on"}},
		AltParamChoiceLabeled{{{to_mm<72>(154.79), to_mm<72>(194.38), Center, "Fire On Unmute ch3", ""}, 2, 1}, {"off", "on"}},
		AltParamChoiceLabeled{{{to_mm<72>(154.79), to_mm<72>(269.98), Center, "Fire On Unmute ch4", ""}, 2, 1}, {"off", "on"}},
		AltParamChoiceLabeled{{{to_mm<72>(151.258), to_mm<72>(17.009), Center, "Output Range", ""}, 2, 1}, {"0V to +10V", "-5V to +5V"}},
}};

    enum class Elem {
        Skew1Knob,
        Ping1Button,
        On1Button,
        Skew2Knob,
        Ping2Button,
        On2Button,
        Skew3Knob,
        Ping3Button,
        On3Button,
        Skew4Knob,
        Ping4Button,
        On4Button,
        Ping1JackIn,
        Skew1CvIn,
        Out1Out,
        Reset1In,
        Ping2JackIn,
        Skew2JackIn,
        Out2Out,
        Reset2In,
        Ping3JackIn,
        Skew3JackIn,
        Out3Out,
        Reset3In,
        Ping4JackIn,
        Skew4JackIn,
        Out4Out,
        Reset4In,
        Led1Light,
        Led2Light,
        Led3Light,
        Led4Light,
        FireOnUnmuteCh1AltParam,
        FireOnUnmuteCh2AltParam,
        FireOnUnmuteCh3AltParam,
        FireOnUnmuteCh4AltParam,
        OutputRangeAltParam,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobSkew_1, 
        KnobSkew_2, 
        KnobSkew_3, 
        KnobSkew_4, 
        NumKnobs,
    };
    
    enum {
        SwitchPing_1, 
        SwitchOn_1, 
        SwitchPing_2, 
        SwitchOn_2, 
        SwitchPing_3, 
        SwitchOn_3, 
        SwitchPing_4, 
        SwitchOn_4, 
        NumSwitches,
    };
    
    enum {
        InputPing_1_Jack, 
        InputSkew_1_Cv, 
        InputReset_1, 
        InputPing_2_Jack, 
        InputSkew_2_Jack, 
        InputReset_2, 
        InputPing_3_Jack, 
        InputSkew_3_Jack, 
        InputReset_3, 
        InputPing_4_Jack, 
        InputSkew_4_Jack, 
        InputReset_4, 
        NumInJacks,
    };
    
    enum {
        OutputOut_1, 
        OutputOut_2, 
        OutputOut_3, 
        OutputOut_4, 
        NumOutJacks,
    };
    
    enum {
        LedLed_1, 
        LedLed_2, 
        LedLed_3, 
        LedLed_4, 
        NumDiscreteLeds,
    };
    
    enum {
        AltParamFire_On_Unmute_Ch1, 
        AltParamFire_On_Unmute_Ch2, 
        AltParamFire_On_Unmute_Ch3, 
        AltParamFire_On_Unmute_Ch4, 
        AltParamOutput_Range, 
    };
};
} // namespace MetaModule
