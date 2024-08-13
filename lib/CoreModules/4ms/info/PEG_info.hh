#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct PEGInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"PEG"};
    static constexpr std::string_view description{"Pingable Envelope Generator"};
    static constexpr uint32_t width_hp = 5;
    static constexpr std::string_view svg_filename{"res/modules/PEG_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/PEG.png"};

    using enum Coords;

    static constexpr std::array<Element, 52> Elements{{
		MomentaryRGB7mm{{to_mm<72>(68.06), to_mm<72>(45.56), Center, "Ping Red", ""}},
		MomentaryRGB7mm{{to_mm<72>(120.9), to_mm<72>(41.96), Center, "Cycle Red", ""}},
		Davies1900hBlackKnob{{to_mm<72>(34.2), to_mm<72>(106.06), Center, "Ping Div Mult Red", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(34.16), to_mm<72>(170.68), Center, "Scale Red", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(95.87), to_mm<72>(151.47), Center, "Skew Red", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(106.67), to_mm<72>(215.41), Center, "Curve Red", ""}, 0.5f},
		OrangeButton{{to_mm<72>(21.56), to_mm<72>(213.99), Center, "Bi-polar Red", ""}},
		MomentaryRGB7mm{{to_mm<72>(220.56), to_mm<72>(45.56), Center, "Ping Blue", ""}},
		MomentaryRGB7mm{{to_mm<72>(167.48), to_mm<72>(41.9), Center, "Cycle Blue", ""}},
		Davies1900hBlackKnob{{to_mm<72>(254.32), to_mm<72>(106.02), Center, "Ping Div Mult Blue", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(254.75), to_mm<72>(170.82), Center, "Scale Blue", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(192.91), to_mm<72>(151.47), Center, "Skew Blue", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(182.51), to_mm<72>(215.41), Center, "Curve Blue", ""}, 0.5f},
		OrangeButton{{to_mm<72>(267.04), to_mm<72>(213.99), Center, "Bi-polar Blue", ""}},
		GateJackInput4ms{{to_mm<72>(21.57), to_mm<72>(45.55), Center, "Ping Red Jack", ""}},
		GateJackInput4ms{{to_mm<72>(267.05), to_mm<72>(45.55), Center, "Ping Blue Jack", ""}},
		GateJackInput4ms{{to_mm<72>(97.57), to_mm<72>(74.03), Center, "Qnt Red", ""}},
		GateJackInput4ms{{to_mm<72>(144.31), to_mm<72>(74.03), Center, "Toggle", ""}},
		GateJackInput4ms{{to_mm<72>(190.86), to_mm<72>(74.03), Center, "Qnt Blue", ""}},
		GateJackInput4ms{{to_mm<72>(120.8), to_mm<72>(107.7), Center, "Async Red", ""}},
		GateJackInput4ms{{to_mm<72>(167.56), to_mm<72>(107.7), Center, "Async Blue", ""}},
		AnalogJackOutput4ms{{to_mm<72>(48.56), to_mm<72>(248.86), Center, "Env Red", ""}},
		AnalogJackOutput4ms{{to_mm<72>(239.76), to_mm<72>(248.86), Center, "Env Blue", ""}},
		AnalogJackOutput4ms{{to_mm<72>(27.7), to_mm<72>(281.13), Center, "+5V Env Red", ""}},
		GateJackOutput4ms{{to_mm<72>(69.36), to_mm<72>(281.13), Center, "EOR", ""}},
		GateJackOutput4ms{{to_mm<72>(111.11), to_mm<72>(281.13), Center, "EOF Red", ""}},
		AnalogJackOutput4ms{{to_mm<72>(144.22), to_mm<72>(299.87), Center, "OR", ""}},
		GateJackOutput4ms{{to_mm<72>(177.5), to_mm<72>(281.13), Center, "EOF Blue", ""}},
		GateJackOutput4ms{{to_mm<72>(219.06), to_mm<72>(280.99), Center, "Half R", ""}},
		AnalogJackOutput4ms{{to_mm<72>(260.81), to_mm<72>(280.99), Center, "+5V Env Blue", ""}},
		AnalogJackInput4ms{{to_mm<72>(27.7), to_mm<72>(317.08), Center, "Div Red Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(69.36), to_mm<72>(317.08), Center, "Skew Red Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(111.11), to_mm<72>(317.08), Center, "Curve Red Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(177.5), to_mm<72>(317.08), Center, "Curve Blue Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(219.06), to_mm<72>(317.08), Center, "Skew Blue Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(260.81), to_mm<72>(317.08), Center, "Div Blue Jack", ""}},
		WhiteLight{{to_mm<72>(47.9), to_mm<72>(213.99), Center, "EnvRedLight", ""}},
		WhiteLight{{to_mm<72>(239.76), to_mm<72>(213.99), Center, "EnvBlueLight", ""}},
		RedLight{{to_mm<72>(73.01), to_mm<72>(258.36), Center, "EORRedLight", ""}},
		RedLight{{to_mm<72>(111.11), to_mm<72>(258.36), Center, "EOFRedLight", ""}},
		BlueLight{{to_mm<72>(177.5), to_mm<72>(258.36), Center, "EOFBlue", ""}},
		BlueLight{{to_mm<72>(215.46), to_mm<72>(258.36), Center, "HalfRiseBlue", ""}},
		AltParamChoiceLabeled{{{to_mm<72>(120.8), to_mm<72>(107.7), Center, "Async Red Mode", ""}, 2, 1}, {"ASR", "AR"}},
		AltParamChoiceLabeled{{{to_mm<72>(167.56), to_mm<72>(107.7), Center, "Async Blue Mode", ""}, 2, 1}, {"ASR", "AR"}},
		AltParamChoiceLabeled{{{to_mm<72>(68.06), to_mm<72>(45.56), Center, "Free-Running Ping Red", ""}, 2, 1}, {"On", "Off"}},
		AltParamChoiceLabeled{{{to_mm<72>(220.56), to_mm<72>(45.56), Center, "Free-Running Ping Blue", ""}, 2, 1}, {"On", "Off"}},
		AltParamChoiceLabeled{{{to_mm<72>(48.56), to_mm<72>(248.86), Center, "Skew Limit Red", ""}, 2, 1}, {"On", "Off"}},
		AltParamChoiceLabeled{{{to_mm<72>(239.76), to_mm<72>(248.86), Center, "Skew Limit Blue", ""}, 2, 1}, {"On", "Off"}},
		AltParamChoiceLabeled{{{to_mm<72>(111.11), to_mm<72>(281.13), Center, "EOF Red Mode", ""}, 4, 1}, {"EOF Gate", "EOF Trig", "Tap Tempo Gate", "Tap Tempo Trig"}},
		AltParamChoiceLabeled{{{to_mm<72>(177.5), to_mm<72>(281.13), Center, "EOF Blue Mode", ""}, 4, 1}, {"EOF Gate", "EOF Trig", "Tap Tempo Gate", "Tap Tempo Trig"}},
		AltParamChoiceLabeled{{{to_mm<72>(69.36), to_mm<72>(281.13), Center, "EOR Red Mode", ""}, 4, 1}, {"EOR Gate", "EOR Trig", "Half-Rise Gate", "Half-Rise Trig"}},
		AltParamChoiceLabeled{{{to_mm<72>(219.06), to_mm<72>(280.99), Center, "Half-R Blue Mode", ""}, 4, 1}, {"Half-Rise Gate", "Half-Rise Trig", "EOR Gate", "EOR Trig"}},
}};

    enum class Elem {
        PingRedButton,
        CycleRedButton,
        PingDivMultRedKnob,
        ScaleRedKnob,
        SkewRedKnob,
        CurveRedKnob,
        BiNPolarRedButton,
        PingBlueButton,
        CycleBlueButton,
        PingDivMultBlueKnob,
        ScaleBlueKnob,
        SkewBlueKnob,
        CurveBlueKnob,
        BiNPolarBlueButton,
        PingRedJackIn,
        PingBlueJackIn,
        QntRedIn,
        ToggleIn,
        QntBlueIn,
        AsyncRedIn,
        AsyncBlueIn,
        EnvRedOut,
        EnvBlueOut,
        P5VEnvRedOut,
        EorOut,
        EofRedOut,
        OrOut,
        EofBlueOut,
        HalfROut,
        P5VEnvBlueOut,
        DivRedJackIn,
        SkewRedJackIn,
        CurveRedJackIn,
        CurveBlueJackIn,
        SkewBlueJackIn,
        DivBlueJackIn,
        EnvredlightLight,
        EnvbluelightLight,
        EorredlightLight,
        EofredlightLight,
        EofblueLight,
        HalfriseblueLight,
        AsyncRedModeAltParam,
        AsyncBlueModeAltParam,
        FreeNRunningPingRedAltParam,
        FreeNRunningPingBlueAltParam,
        SkewLimitRedAltParam,
        SkewLimitBlueAltParam,
        EofRedModeAltParam,
        EofBlueModeAltParam,
        EorRedModeAltParam,
        HalfNRBlueModeAltParam,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobPing_Div_Mult_Red, 
        KnobScale_Red, 
        KnobSkew_Red, 
        KnobCurve_Red, 
        KnobPing_Div_Mult_Blue, 
        KnobScale_Blue, 
        KnobSkew_Blue, 
        KnobCurve_Blue, 
        NumKnobs,
    };
    
    enum {
        SwitchPing_Red, 
        SwitchCycle_Red, 
        SwitchBiNPolar_Red, 
        SwitchPing_Blue, 
        SwitchCycle_Blue, 
        SwitchBiNPolar_Blue, 
        NumSwitches,
    };
    
    enum {
        InputPing_Red_Jack, 
        InputPing_Blue_Jack, 
        InputQnt_Red, 
        InputToggle, 
        InputQnt_Blue, 
        InputAsync_Red, 
        InputAsync_Blue, 
        InputDiv_Red_Jack, 
        InputSkew_Red_Jack, 
        InputCurve_Red_Jack, 
        InputCurve_Blue_Jack, 
        InputSkew_Blue_Jack, 
        InputDiv_Blue_Jack, 
        NumInJacks,
    };
    
    enum {
        OutputEnv_Red, 
        OutputEnv_Blue, 
        OutputP5V_Env_Red, 
        OutputEor, 
        OutputEof_Red, 
        OutputOr, 
        OutputEof_Blue, 
        OutputHalf_R, 
        OutputP5V_Env_Blue, 
        NumOutJacks,
    };
    
    enum {
        LedEnvredlight, 
        LedEnvbluelight, 
        LedEorredlight, 
        LedEofredlight, 
        LedEofblue, 
        LedHalfriseblue, 
        NumDiscreteLeds,
    };
    
    enum {
        AltParamAsync_Red_Mode, 
        AltParamAsync_Blue_Mode, 
        AltParamFreeNRunning_Ping_Red, 
        AltParamFreeNRunning_Ping_Blue, 
        AltParamSkew_Limit_Red, 
        AltParamSkew_Limit_Blue, 
        AltParamEof_Red_Mode, 
        AltParamEof_Blue_Mode, 
        AltParamEor_Red_Mode, 
        AltParamHalfNR_Blue_Mode, 
    };
};
} // namespace MetaModule
