#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct SCMInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"SCM"};
    static constexpr std::string_view description{"Shuffling Clock Multiplier"};
    static constexpr uint32_t width_hp = 12;
    static constexpr std::string_view svg_filename{"res/modules/SCM_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/SCM.png"};

    using enum Coords;

    static constexpr std::array<Element, 33> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(147.99), to_mm<72>(54.12), Center, "Rotate", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(147.99), to_mm<72>(104.09), Center, "Slip", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(147.99), to_mm<72>(154.07), Center, "Shuffle", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(147.99), to_mm<72>(204.04), Center, "Skip", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(147.99), to_mm<72>(254.01), Center, "PW", ""}, 0.5f},
		OrangeButton{{to_mm<72>(148.84), to_mm<72>(291.85), Center, "4x Fast", ""}},
		OrangeButton{{to_mm<72>(148.84), to_mm<72>(325.47), Center, "Mute", ""}},
		GateJackInput4ms{{to_mm<72>(46.48), to_mm<72>(56.3), Center, "Clk In", ""}},
		GateJackInput4ms{{to_mm<72>(81.45), to_mm<72>(55.11), Center, "Resync", ""}},
		GateJackOutput4ms{{to_mm<72>(46.48), to_mm<72>(87.91), Center, "x1", ""}},
		AnalogJackInput4ms{{to_mm<72>(81.45), to_mm<72>(90.12), Center, "Rotate Jack", ""}},
		GateJackOutput4ms{{to_mm<72>(46.48), to_mm<72>(119.52), Center, "x2", ""}},
		AnalogJackInput4ms{{to_mm<72>(81.45), to_mm<72>(126.12), Center, "Slip Jack", ""}},
		GateJackOutput4ms{{to_mm<72>(46.48), to_mm<72>(151.13), Center, "S3", ""}},
		AnalogJackInput4ms{{to_mm<72>(81.45), to_mm<72>(162.12), Center, "Shuffle Jack", ""}},
		GateJackOutput4ms{{to_mm<72>(46.48), to_mm<72>(182.73), Center, "S4", ""}},
		AnalogJackInput4ms{{to_mm<72>(81.45), to_mm<72>(198.12), Center, "Skip Jack", ""}},
		GateJackOutput4ms{{to_mm<72>(46.48), to_mm<72>(214.34), Center, "S5", ""}},
		AnalogJackInput4ms{{to_mm<72>(81.45), to_mm<72>(234.12), Center, "PW Jack", ""}},
		GateJackOutput4ms{{to_mm<72>(46.48), to_mm<72>(245.95), Center, "S6", ""}},
		GateJackInput4ms{{to_mm<72>(81.45), to_mm<72>(270.12), Center, "4x Fast Jack", ""}},
		GateJackOutput4ms{{to_mm<72>(46.48), to_mm<72>(277.56), Center, "S8", ""}},
		GateJackInput4ms{{to_mm<72>(81.45), to_mm<72>(306.12), Center, "Mute Jack", ""}},
		GateJackOutput4ms{{to_mm<72>(46.48), to_mm<72>(309.17), Center, "x8", ""}},
		WhiteLight{{to_mm<72>(17.3), to_mm<72>(56.3), Center, "LED in", ""}},
		BlueLight{{to_mm<72>(17.3), to_mm<72>(87.91), Center, "LED x1", ""}},
		BlueLight{{to_mm<72>(17.3), to_mm<72>(119.52), Center, "LED x2", ""}},
		GreenLight{{to_mm<72>(17.3), to_mm<72>(151.13), Center, "LED S3", ""}},
		GreenLight{{to_mm<72>(17.3), to_mm<72>(182.73), Center, "LED S4", ""}},
		GreenLight{{to_mm<72>(17.3), to_mm<72>(214.34), Center, "LED S5", ""}},
		GreenLight{{to_mm<72>(17.3), to_mm<72>(245.95), Center, "LED S6", ""}},
		GreenLight{{to_mm<72>(17.3), to_mm<72>(277.56), Center, "LED S8", ""}},
		BlueLight{{to_mm<72>(17.3), to_mm<72>(309.17), Center, "LED x8", ""}},
}};

    enum class Elem {
        RotateKnob,
        SlipKnob,
        ShuffleKnob,
        SkipKnob,
        PwKnob,
        _4XFastButton,
        MuteButton,
        ClkIn,
        ResyncIn,
        X1Out,
        RotateJackIn,
        X2Out,
        SlipJackIn,
        S3Out,
        ShuffleJackIn,
        S4Out,
        SkipJackIn,
        S5Out,
        PwJackIn,
        S6Out,
        _4XFastJackIn,
        S8Out,
        MuteJackIn,
        X8Out,
        LedInLight,
        LedX1Light,
        LedX2Light,
        LedS3Light,
        LedS4Light,
        LedS5Light,
        LedS6Light,
        LedS8Light,
        LedX8Light,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobRotate, 
        KnobSlip, 
        KnobShuffle, 
        KnobSkip, 
        KnobPw, 
        NumKnobs,
    };
    
    enum {
        Switch_4X_Fast, 
        SwitchMute, 
        NumSwitches,
    };
    
    enum {
        InputClk_In, 
        InputResync, 
        InputRotate_Jack, 
        InputSlip_Jack, 
        InputShuffle_Jack, 
        InputSkip_Jack, 
        InputPw_Jack, 
        Input_4X_Fast_Jack, 
        InputMute_Jack, 
        NumInJacks,
    };
    
    enum {
        OutputX1, 
        OutputX2, 
        OutputS3, 
        OutputS4, 
        OutputS5, 
        OutputS6, 
        OutputS8, 
        OutputX8, 
        NumOutJacks,
    };
    
    enum {
        LedLed_In, 
        LedLed_X1, 
        LedLed_X2, 
        LedLed_S3, 
        LedLed_S4, 
        LedLed_S5, 
        LedLed_S6, 
        LedLed_S8, 
        LedLed_X8, 
        NumDiscreteLeds,
    };
    
};
} // namespace MetaModule
