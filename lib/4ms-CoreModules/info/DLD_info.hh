#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct DLDInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"DLD"};
    static constexpr std::string_view description{"Dual Looping Delay"};
    static constexpr uint32_t width_hp = 20;
    static constexpr std::string_view svg_filename{"res/modules/DLD_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/DLD.png"};

    using enum Coords;

    static constexpr std::array<Element, 56> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(34.32), to_mm<72>(53.53), Center, "Time A", ""}, 0.5f},
		Toggle3pos{{to_mm<72>(84.11), to_mm<72>(51.455), Center, "Time Mult A", ""}, {"1/8th", "=", "+16"}, Toggle3pos::State_t::CENTER},
		WhiteMomentary7mm{{to_mm<72>(124.03), to_mm<72>(42.51), Center, "Ping Button", ""}},
		Toggle3pos{{to_mm<72>(205.35), to_mm<72>(51.455), Center, "Time Mult B", ""}, {"1/8th", "=", "+16"}, Toggle3pos::State_t::CENTER},
		Davies1900hBlackKnob{{to_mm<72>(254.17), to_mm<72>(53.39), Center, "Time B", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(34.17), to_mm<72>(119.05), Center, "Feedback A", ""}, 0.375f},
		WhiteMomentary7mm{{to_mm<72>(81.85), to_mm<72>(123.11), Center, "Reverse A", ""}},
		WhiteMomentary7mm{{to_mm<72>(118.44), to_mm<72>(135.74), Center, "Hold A", ""}},
		WhiteMomentary7mm{{to_mm<72>(169.66), to_mm<72>(135.74), Center, "Hold B", ""}},
		WhiteMomentary7mm{{to_mm<72>(206.08), to_mm<72>(123.11), Center, "Reverse B", ""}},
		Davies1900hBlackKnob{{to_mm<72>(254.59), to_mm<72>(119.05), Center, "Feedback B", ""}, 0.375f},
		Davies1900hBlackKnob{{to_mm<72>(43.46), to_mm<72>(184.04), Center, "Delay Feed A", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(111.56), to_mm<72>(178.84), Center, "Mix A", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(177.02), to_mm<72>(178.91), Center, "Mix B", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(245.19), to_mm<72>(183.78), Center, "Delay Feed B", ""}, 1.0f},
		AnalogJackInput4ms{{to_mm<72>(167.59), to_mm<72>(42.51), Center, "Ping Jack", ""}},
		AnalogJackOutput4ms{{to_mm<72>(108.25), to_mm<72>(81.44), Center, "Loop A", ""}},
		AnalogJackOutput4ms{{to_mm<72>(144.16), to_mm<72>(93.48), Center, "Clock Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(180.38), to_mm<72>(81.44), Center, "Loop B", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.08), to_mm<72>(242.1), Center, "In A", ""}},
		AnalogJackInput4ms{{to_mm<72>(71.16), to_mm<72>(242.1), Center, "Return A", ""}},
		GateJackInput4ms{{to_mm<72>(122.51), to_mm<72>(242.1), Center, "Reverse A Jack", ""}},
		GateJackInput4ms{{to_mm<72>(165.98), to_mm<72>(242.1), Center, "Reverse B Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(217.15), to_mm<72>(242.1), Center, "Return B", ""}},
		AnalogJackInput4ms{{to_mm<72>(260.18), to_mm<72>(242.1), Center, "In B", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.08), to_mm<72>(281.49), Center, "Out A", ""}},
		AnalogJackOutput4ms{{to_mm<72>(71.16), to_mm<72>(281.49), Center, "Send A", ""}},
		GateJackInput4ms{{to_mm<72>(122.51), to_mm<72>(281.49), Center, "Hold A Jack", ""}},
		GateJackInput4ms{{to_mm<72>(165.98), to_mm<72>(281.49), Center, "Hold B Jack", ""}},
		AnalogJackOutput4ms{{to_mm<72>(217.15), to_mm<72>(281.49), Center, "Send B", ""}},
		AnalogJackOutput4ms{{to_mm<72>(260.18), to_mm<72>(281.49), Center, "Out B", ""}},
		AnalogJackInput4ms{{to_mm<72>(25.58), to_mm<72>(322.03), Center, "Time A Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(72.88), to_mm<72>(322.03), Center, "Feedback A Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(120.17), to_mm<72>(322.03), Center, "Delay Feed A Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(167.84), to_mm<72>(322.03), Center, "Delay Feed B Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(215.27), to_mm<72>(322.03), Center, "Feedback B Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(262.73), to_mm<72>(322.03), Center, "Time B Jack", ""}},
		RedLight{{to_mm<72>(66.03), to_mm<72>(34.94), Center, "Time A LED", ""}},
		RedLight{{to_mm<72>(222.2), to_mm<72>(34.94), Center, "Time B LED", ""}},
		AltParamChoiceLabeled{{{to_mm<72>(29.0), to_mm<72>(13.0), Center, "Soft Clip A", ""}, 2, 1}, {"On", "Off"}},
		AltParamChoiceLabeled{{{to_mm<72>(229.0), to_mm<72>(13.0), Center, "Soft Clip B", ""}, 2, 1}, {"On", "Off"}},
		AltParamChoiceLabeled{{{to_mm<72>(12.0), to_mm<72>(13.0), Center, "AutoMute A", ""}, 2, 1}, {"On", "Off"}},
		AltParamChoiceLabeled{{{to_mm<72>(212.0), to_mm<72>(13.0), Center, "AutoMute B", ""}, 2, 1}, {"On", "Off"}},
		AltParamChoiceLabeled{{{to_mm<72>(167.59), to_mm<72>(42.51), Center, "Ping Method", ""}, 5, 1}, {"Avg 4", "Avg 2", "1:1", "IgnoreJitter", "Ignore%Change"}},
		AltParamChoiceLabeled{{{to_mm<72>(122.51), to_mm<72>(281.49), Center, "Hold A", ""}, 2, 1}, {"Trig", "Gate"}},
		AltParamChoiceLabeled{{{to_mm<72>(165.98), to_mm<72>(281.49), Center, "Hold B", ""}, 2, 1}, {"Trig", "Gate"}},
		AltParamChoiceLabeled{{{to_mm<72>(122.51), to_mm<72>(242.1), Center, "Rev. A", ""}, 2, 1}, {"Trig", "Gate"}},
		AltParamChoiceLabeled{{{to_mm<72>(165.98), to_mm<72>(242.1), Center, "Rev. B", ""}, 2, 1}, {"Trig", "Gate"}},
		AltParamChoiceLabeled{{{to_mm<72>(43.46), to_mm<72>(184.04), Center, "DelayFeed A Taper", ""}, 2, 1}, {"Log", "Lin"}},
		AltParamChoiceLabeled{{{to_mm<72>(245.19), to_mm<72>(183.78), Center, "DelayFeed B Taper", ""}, 2, 1}, {"Log", "Lin"}},
		AltParamChoiceLabeled{{{to_mm<72>(25.58), to_mm<72>(322.03), Center, "Time A Auto-UnQ", ""}, 2, 1}, {"Off", "On"}},
		AltParamChoiceLabeled{{{to_mm<72>(262.73), to_mm<72>(322.03), Center, "Time B Auto-UnQ", ""}, 2, 1}, {"Off", "On"}},
		AltParamChoiceLabeled{{{to_mm<72>(71.16), to_mm<72>(242.1), Center, "Stereo Mode A", ""}, 2, 1}, {"Off", "On"}},
		AltParamChoiceLabeled{{{to_mm<72>(217.15), to_mm<72>(242.1), Center, "Stereo Mode B", ""}, 2, 1}, {"Off", "On"}},
		AltParamChoiceLabeled{{{to_mm<72>(34.32), to_mm<72>(53.53), Center, "Crossfade A", ""}, 7, 4}, {"1ms", "2ms", "4ms", "8ms", "25ms", "100ms", "250ms"}},
		AltParamChoiceLabeled{{{to_mm<72>(254.17), to_mm<72>(53.39), Center, "Crossfade B", ""}, 7, 4}, {"1ms", "2ms", "4ms", "8ms", "25ms", "100ms", "250ms"}},
}};

    enum class Elem {
        TimeAKnob,
        TimeMultASwitch,
        PingButton,
        TimeMultBSwitch,
        TimeBKnob,
        FeedbackAKnob,
        ReverseAButton,
        HoldAButton,
        HoldBButton,
        ReverseBButton,
        FeedbackBKnob,
        DelayFeedAKnob,
        MixAKnob,
        MixBKnob,
        DelayFeedBKnob,
        PingJackIn,
        LoopAOut,
        ClockOut,
        LoopBOut,
        InAIn,
        ReturnAIn,
        ReverseAJackIn,
        ReverseBJackIn,
        ReturnBIn,
        InBIn,
        OutAOut,
        SendAOut,
        HoldAJackIn,
        HoldBJackIn,
        SendBOut,
        OutBOut,
        TimeAJackIn,
        FeedbackAJackIn,
        DelayFeedAJackIn,
        DelayFeedBJackIn,
        FeedbackBJackIn,
        TimeBJackIn,
        TimeALedLight,
        TimeBLedLight,
        SoftClipAAltParam,
        SoftClipBAltParam,
        AutomuteAAltParam,
        AutomuteBAltParam,
        PingMethodAltParam,
        HoldAAltParam,
        HoldBAltParam,
        Rev_AAltParam,
        Rev_BAltParam,
        DelayfeedATaperAltParam,
        DelayfeedBTaperAltParam,
        TimeAAutoNUnqAltParam,
        TimeBAutoNUnqAltParam,
        StereoModeAAltParam,
        StereoModeBAltParam,
        CrossfadeAAltParam,
        CrossfadeBAltParam,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobTime_A, 
        KnobTime_B, 
        KnobFeedback_A, 
        KnobFeedback_B, 
        KnobDelay_Feed_A, 
        KnobMix_A, 
        KnobMix_B, 
        KnobDelay_Feed_B, 
        NumKnobs,
    };
    
    enum {
        SwitchTime_Mult_A, 
        SwitchPing_Button, 
        SwitchTime_Mult_B, 
        SwitchReverse_A, 
        SwitchHold_A, 
        SwitchHold_B, 
        SwitchReverse_B, 
        NumSwitches,
    };
    
    enum {
        InputPing_Jack, 
        InputIn_A, 
        InputReturn_A, 
        InputReverse_A_Jack, 
        InputReverse_B_Jack, 
        InputReturn_B, 
        InputIn_B, 
        InputHold_A_Jack, 
        InputHold_B_Jack, 
        InputTime_A_Jack, 
        InputFeedback_A_Jack, 
        InputDelay_Feed_A_Jack, 
        InputDelay_Feed_B_Jack, 
        InputFeedback_B_Jack, 
        InputTime_B_Jack, 
        NumInJacks,
    };
    
    enum {
        OutputLoop_A, 
        OutputClock_Out, 
        OutputLoop_B, 
        OutputOut_A, 
        OutputSend_A, 
        OutputSend_B, 
        OutputOut_B, 
        NumOutJacks,
    };
    
    enum {
        LedTime_A_Led, 
        LedTime_B_Led, 
        NumDiscreteLeds,
    };
    
    enum {
        AltParamSoft_Clip_A, 
        AltParamSoft_Clip_B, 
        AltParamAutomute_A, 
        AltParamAutomute_B, 
        AltParamPing_Method, 
        AltParamHold_A, 
        AltParamHold_B, 
        AltParamRev__A, 
        AltParamRev__B, 
        AltParamDelayfeed_A_Taper, 
        AltParamDelayfeed_B_Taper, 
        AltParamTime_A_AutoNUnq, 
        AltParamTime_B_AutoNUnq, 
        AltParamStereo_Mode_A, 
        AltParamStereo_Mode_B, 
        AltParamCrossfade_A, 
        AltParamCrossfade_B, 
    };
};
} // namespace MetaModule
