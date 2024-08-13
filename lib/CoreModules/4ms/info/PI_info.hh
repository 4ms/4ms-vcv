#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct PIInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"PI"};
    static constexpr std::string_view description{"Percussion Interface"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/PI_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/PI.png"};

    using enum Coords;

    static constexpr std::array<Element, 18> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(30.9), to_mm<72>(54.04), Center, "Sensitivity", ""}},
		Knob9mm{{to_mm<72>(86.53), to_mm<72>(46.58), Center, "Env. Level", ""}},
		Knob9mm{{to_mm<72>(30.8), to_mm<72>(109.3), Center, "Sustain", ""}},
		Knob9mm{{to_mm<72>(86.53), to_mm<72>(97.941), Center, "Inv. Level", ""}},
		Knob9mm{{to_mm<72>(30.8), to_mm<72>(153.17), Center, "Env. Decay", ""}},
		Toggle3posHoriz{{to_mm<72>(86.53), to_mm<72>(145.308), Center, "Gain", ""}, {"Low", "Med", "High"}},
		Toggle2posHoriz{{to_mm<72>(30.8), to_mm<72>(195.475), Center, "Env Mode", ""}, {"Follow", "Gen"}},
		AnalogJackOutput4ms{{to_mm<72>(86.52), to_mm<72>(196.113), Center, "Env. Out", ""}},
		AnalogJackInput4ms{{to_mm<72>(19.83), to_mm<72>(221.47), Center, "IN", ""}},
		AnalogJackOutput4ms{{to_mm<72>(86.52), to_mm<72>(248.361), Center, "Inv. Out", ""}},
		GateJackOutput4ms{{to_mm<72>(41.83), to_mm<72>(253.56), Center, "Gate Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(86.52), to_mm<72>(298.075), Center, "Audio Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(19.95), to_mm<72>(286.08), Center, "Env. Out +", ""}},
		AnalogJackOutput4ms{{to_mm<72>(41.83), to_mm<72>(318.61), Center, "Env. Out -", ""}},
		RedGreenBlueLight{{to_mm<72>(11.39), to_mm<72>(32.64), Center, "Sens. Light", ""}},
		WhiteLight{{to_mm<72>(50.37), to_mm<72>(230.46), Center, "Gate Light", ""}},
		BlueLight{{to_mm<72>(10.77), to_mm<72>(261.2), Center, "Env + Light", ""}},
		GreenLight{{to_mm<72>(50.37), to_mm<72>(292.62), Center, "Env - Light", ""}},
}};

    enum class Elem {
        SensitivityKnob,
        Env_LevelKnob,
        SustainKnob,
        Inv_LevelKnob,
        Env_DecayKnob,
        GainSwitch,
        EnvModeSwitch,
        Env_Out,
        In,
        Inv_Out,
        GateOut,
        AudioOut,
        Env_OutPOut,
        Env_OutNOut,
        Sens_Light,
        GateLight,
        EnvPLight,
        EnvNLight,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobSensitivity, 
        KnobEnv__Level, 
        KnobSustain, 
        KnobInv__Level, 
        KnobEnv__Decay, 
        NumKnobs,
    };
    
    enum {
        SwitchGain, 
        SwitchEnv_Mode, 
        NumSwitches,
    };
    
    enum {
        InputIn, 
        NumInJacks,
    };
    
    enum {
        OutputEnv__Out, 
        OutputInv__Out, 
        OutputGate_Out, 
        OutputAudio_Out, 
        OutputEnv__Out_P, 
        OutputEnv__Out_N, 
        NumOutJacks,
    };
    
    enum {
        LedSens__Light, 
        LedGate_Light, 
        LedEnv_P_Light, 
        LedEnv_N_Light, 
        NumDiscreteLeds,
    };
    
};
} // namespace MetaModule
