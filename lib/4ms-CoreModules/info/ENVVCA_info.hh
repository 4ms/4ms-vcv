#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct ENVVCAInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"ENVVCA"};
    static constexpr std::string_view description{"EnvelopeVCA"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/ENVVCA_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/ENVVCA.png"};

    using enum Coords;

    static constexpr std::array<Element, 19> Elements{{
		Toggle3pos{{to_mm<72>(23.19), to_mm<72>(43.305), Center, "Rise Switch", ""}, {"Fast", "Med", "Slow"}, Toggle3pos::State_t::CENTER},
		Toggle3pos{{to_mm<72>(57.33), to_mm<72>(43.305), Center, "Fall Switch", ""}, {"Fast", "Med", "Slow"}, Toggle3pos::State_t::CENTER},
		OrangeButton{{to_mm<72>(92.17), to_mm<72>(41.65), Center, "Cycle", ""}},
		Slider25mmVertLED{{to_mm<72>(23.185), to_mm<72>(108.81), Center, "Rise Slider", ""}, 0.5f},
		Slider25mmVertLED{{to_mm<72>(57.325), to_mm<72>(109.02), Center, "Fall Slider", ""}, 0.5f},
		Slider25mmVertLED{{to_mm<72>(91.505), to_mm<72>(108.81), Center, "Env Level Slider", ""}, 1.0f},
		Knob9mm{{to_mm<72>(21.69), to_mm<72>(178.25), Center, "Rise CV", ""}, 1.0f},
		Knob9mm{{to_mm<72>(92.85), to_mm<72>(178.25), Center, "Fall CV", ""}, 1.0f},
		AnalogJackInput4ms{{to_mm<72>(57.25), to_mm<72>(203.53), Center, "Time CV", ""}},
		GateJackInput4ms{{to_mm<72>(22.3), to_mm<72>(227.06), Center, "Trigger", ""}},
		AnalogJackOutput4ms{{to_mm<72>(92.03), to_mm<72>(227.06), Center, "Env", ""}},
		GateJackInput4ms{{to_mm<72>(57.25), to_mm<72>(254.25), Center, "Cycle", ""}},
		AnalogJackInput4ms{{to_mm<72>(22.3), to_mm<72>(278.73), Center, "Follow", ""}},
		GateJackOutput4ms{{to_mm<72>(92.03), to_mm<72>(278.73), Center, "EOR", ""}},
		AnalogJackInput4ms{{to_mm<72>(35.87), to_mm<72>(322.47), Center, "Audio In", ""}},
		AnalogJackOutput4ms{{to_mm<72>(78.57), to_mm<72>(322.5), Center, "Audio Out", ""}},
		RedBlueLight{{to_mm<72>(45.11), to_mm<72>(174.84), Center, "Rise Light", ""}},
		RedBlueLight{{to_mm<72>(69.34), to_mm<72>(174.84), Center, "Fall Light", ""}},
		OrangeLight{{to_mm<72>(106.41), to_mm<72>(256.6), Center, "EOR Light", ""}},
}};

    enum class Elem {
        RiseSwitch,
        FallSwitch,
        CycleButton,
        RiseSlider,
        FallSlider,
        EnvLevelSlider,
        RiseCvKnob,
        FallCvKnob,
        TimeCvIn,
        TriggerIn,
        EnvOut,
        CycleIn,
        FollowIn,
        EorOut,
        AudioIn,
        AudioOut,
        RiseLight,
        FallLight,
        EorLight,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobRise_Slider, 
        KnobFall_Slider, 
        KnobEnv_Level_Slider, 
        KnobRise_Cv, 
        KnobFall_Cv, 
        NumKnobs,
    };
    
    enum {
        SwitchRise_Switch, 
        SwitchFall_Switch, 
        SwitchCycle, 
        NumSwitches,
    };
    
    enum {
        InputTime_Cv, 
        InputTrigger, 
        InputCycle, 
        InputFollow, 
        InputAudio_In, 
        NumInJacks,
    };
    
    enum {
        OutputEnv, 
        OutputEor, 
        OutputAudio_Out, 
        NumOutJacks,
    };
    
    enum {
        LedRise_Light, 
        LedFall_Light, 
        LedEor_Light, 
        NumDiscreteLeds,
    };
    
};
} // namespace MetaModule
