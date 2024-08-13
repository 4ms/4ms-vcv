#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct MPEGInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"MPEG"};
    static constexpr std::string_view description{"Mini PEG"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/MPEG_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/MPEG.png"};

    using enum Coords;

    static constexpr std::array<Element, 25> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(36.45), to_mm<72>(87.77), Center, "Div/Mult", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(37.35), to_mm<72>(154.72), Center, "Shape", ""}, 0.5f},
		Knob9mm{{to_mm<72>(91.14), to_mm<72>(133.33), Center, "Scale", ""}, 1.0f},
		Knob9mm{{to_mm<72>(91.14), to_mm<72>(188.54), Center, "Offset", ""}, 1.0f},
		MomentaryRGB7mm{{to_mm<72>(30.52), to_mm<72>(41.57), Center, "Ping", ""}},
		MomentaryRGB7mm{{to_mm<72>(92.15), to_mm<72>(81.15), Center, "Cycle", ""}},
		GateJackInput4ms{{to_mm<72>(80.21), to_mm<72>(41.57), Center, "Ping Jack", ""}},
		GateJackInput4ms{{to_mm<72>(23.01), to_mm<72>(287.64), Center, "Trigger", ""}},
		AnalogJackInput4ms{{to_mm<72>(57.38), to_mm<72>(267.84), Center, "Shape CV", ""}},
		GateJackInput4ms{{to_mm<72>(91.59), to_mm<72>(287.64), Center, "Cycle Trig", ""}},
		AnalogJackInput4ms{{to_mm<72>(57.38), to_mm<72>(311.04), Center, "Div/Mult CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(57.12), to_mm<72>(224.36), Center, "ENV Out", ""}},
		GateJackOutput4ms{{to_mm<72>(23.07), to_mm<72>(244.84), Center, "EOF", ""}},
		AnalogJackOutput4ms{{to_mm<72>(91.7), to_mm<72>(244.94), Center, "5V ENV Out", ""}},
		RedLight{{to_mm<72>(14.57), to_mm<72>(220.0), Center, "EOF Light", ""}},
		RedGreenBlueLight{{to_mm<72>(57.4), to_mm<72>(200.1), Center, "ENV OUT Light", ""}},
		RedGreenBlueLight{{to_mm<72>(99.67), to_mm<72>(220.0), Center, "5V ENV Light", ""}},
		AltParamChoiceLabeled{{{to_mm<72>(30.52), to_mm<72>(41.57), Center, "Sync Mode", ""}, 2, 1}, {"Sync", "Async"}},
		AltParamContinuous{{to_mm<72>(91.14), to_mm<72>(188.54), Center, "Shift", ""}, 0.5f},
		AltParamChoiceLabeled{{{to_mm<72>(57.12), to_mm<72>(224.36), Center, "Skew Limit", ""}, 2, 2}, {"Off", "On"}},
		AltParamChoiceLabeled{{{to_mm<72>(80.21), to_mm<72>(41.57), Center, "Free-running Ping", ""}, 2, 1}, {"On", "Off"}},
		AltParamChoiceLabeled{{{to_mm<72>(23.07), to_mm<72>(244.84), Center, "EOF Jack Type", ""}, 2, 1}, {"Gate", "Trigger"}},
		AltParamChoiceLabeled{{{to_mm<72>(23.07), to_mm<72>(244.84), Center, "EOF Jack Mode", ""}, 4, 2}, {"EOR", "EOF", "Half-Rise", "Tap Clock"}},
		AltParamChoiceLabeled{{{to_mm<72>(91.59), to_mm<72>(287.64), Center, "Cycle Jack Mode", ""}, 3, 1}, {"Trig", "Gate+Sync", "Gate"}},
		AltParamChoiceLabeled{{{to_mm<72>(23.01), to_mm<72>(287.64), Center, "Trig Jack Mode", ""}, 3, 1}, {"Async  Trig", "Async Gate", "Quantized Trig"}},
}};

    enum class Elem {
        Div_MultKnob,
        ShapeKnob,
        ScaleKnob,
        OffsetKnob,
        PingButton,
        CycleButton,
        PingJackIn,
        TriggerIn,
        ShapeCvIn,
        CycleTrigIn,
        Div_MultCvIn,
        EnvOut,
        EofOut,
        _5VEnvOut,
        EofLight,
        EnvOutLight,
        _5VEnvLight,
        SyncModeAltParam,
        ShiftAltParam,
        SkewLimitAltParam,
        FreeNRunningPingAltParam,
        EofJackTypeAltParam,
        EofJackModeAltParam,
        CycleJackModeAltParam,
        TrigJackModeAltParam,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobDiv_Mult, 
        KnobShape, 
        KnobScale, 
        KnobOffset, 
        NumKnobs,
    };
    
    enum {
        SwitchPing, 
        SwitchCycle, 
        NumSwitches,
    };
    
    enum {
        InputPing_Jack, 
        InputTrigger, 
        InputShape_Cv, 
        InputCycle_Trig, 
        InputDiv_Mult_Cv, 
        NumInJacks,
    };
    
    enum {
        OutputEnv_Out, 
        OutputEof, 
        Output_5V_Env_Out, 
        NumOutJacks,
    };
    
    enum {
        LedEof_Light, 
        LedEnv_Out_Light, 
        Led_5V_Env_Light, 
        NumDiscreteLeds,
    };
    
    enum {
        AltParamSync_Mode, 
        AltParamShift, 
        AltParamSkew_Limit, 
        AltParamFreeNRunning_Ping, 
        AltParamEof_Jack_Type, 
        AltParamEof_Jack_Mode, 
        AltParamCycle_Jack_Mode, 
        AltParamTrig_Jack_Mode, 
    };
};
} // namespace MetaModule
