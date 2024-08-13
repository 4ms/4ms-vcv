#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct DEVInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"DEV"};
    static constexpr std::string_view description{"Dual EnvVCA"};
    static constexpr uint32_t width_hp = 16;
    static constexpr std::string_view svg_filename{"res/modules/DEV_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/DEV.png"};

    using enum Coords;

    static constexpr std::array<Element, 44> Elements{{
		Toggle3pos{{to_mm<72>(17.6), to_mm<72>(41.905), Center, "Rise A Switch", ""}, {"Fast", "Med", "Slow"}, Toggle3pos::State_t::CENTER},
		Toggle3pos{{to_mm<72>(50.49), to_mm<72>(41.905), Center, "Fall A Switch", ""}, {"Fast", "Med", "Slow"}, Toggle3pos::State_t::CENTER},
		OrangeButton{{to_mm<72>(82.8), to_mm<72>(41.64), Center, "Cycle A", ""}},
		OrangeButton{{to_mm<72>(147.61), to_mm<72>(41.68), Center, "Cycle B", ""}},
		Toggle3pos{{to_mm<72>(179.89), to_mm<72>(41.905), Center, "Rise B Switch", ""}, {"Fast", "Med", "Slow"}, Toggle3pos::State_t::CENTER},
		Toggle3pos{{to_mm<72>(212.77), to_mm<72>(41.905), Center, "Fall B Switch", ""}, {"Fast", "Med", "Slow"}, Toggle3pos::State_t::CENTER},
		Slider25mmVertLED{{to_mm<72>(22.415), to_mm<72>(108.25), Center, "Rise A Slider", ""}, 0.5f},
		Slider25mmVertLED{{to_mm<72>(56.265), to_mm<72>(108.25), Center, "Fall A Slider", ""}, 0.5f},
		Knob9mm{{to_mm<72>(93.13), to_mm<72>(95.86), Center, "Level A", ""}, 1.0f},
		Knob9mm{{to_mm<72>(137.25), to_mm<72>(95.86), Center, "Level B", ""}, 1.0f},
		Knob9mm{{to_mm<72>(93.13), to_mm<72>(138.65), Center, "Offset A", ""}, 0.5f},
		Knob9mm{{to_mm<72>(137.25), to_mm<72>(138.65), Center, "Offset B", ""}, 0.5f},
		Slider25mmVertLED{{to_mm<72>(174.115), to_mm<72>(108.25), Center, "Rise B Slider", ""}, 0.5f},
		Slider25mmVertLED{{to_mm<72>(207.965), to_mm<72>(108.25), Center, "Fall B Slider", ""}, 0.5f},
		Knob9mm{{to_mm<72>(23.5), to_mm<72>(184.03), Center, "Rise A", ""}, 1.0f},
		Knob9mm{{to_mm<72>(94.11), to_mm<72>(183.67), Center, "Fall A", ""}, 1.0f},
		Knob9mm{{to_mm<72>(136.27), to_mm<72>(184.03), Center, "Rise B", ""}, 1.0f},
		Knob9mm{{to_mm<72>(206.87), to_mm<72>(184.03), Center, "Fall B", ""}, 1.0f},
		GateJackInput4ms{{to_mm<72>(115.19), to_mm<72>(60.85), Center, "Cycle Gate", ""}},
		AnalogJackInput4ms{{to_mm<72>(58.56), to_mm<72>(208.68), Center, "Time CV A", ""}},
		AnalogJackInput4ms{{to_mm<72>(171.86), to_mm<72>(208.68), Center, "Time CV B", ""}},
		GateJackInput4ms{{to_mm<72>(23.09), to_mm<72>(232.82), Center, "Trig A", ""}},
		AnalogJackInput4ms{{to_mm<72>(58.52), to_mm<72>(252.75), Center, "Follow A", ""}},
		GateJackOutput4ms{{to_mm<72>(95.04), to_mm<72>(233.88), Center, "EOR A", ""}},
		GateJackOutput4ms{{to_mm<72>(135.28), to_mm<72>(233.99), Center, "EOF B", ""}},
		AnalogJackInput4ms{{to_mm<72>(171.85), to_mm<72>(252.75), Center, "Follow B", ""}},
		GateJackInput4ms{{to_mm<72>(207.29), to_mm<72>(232.82), Center, "Trig B", ""}},
		AnalogJackInput4ms{{to_mm<72>(23.06), to_mm<72>(274.73), Center, "Audio A In", ""}},
		AnalogJackInput4ms{{to_mm<72>(58.48), to_mm<72>(300.48), Center, "VCA CV A", ""}},
		AnalogJackOutput4ms{{to_mm<72>(115.22), to_mm<72>(288.78), Center, "OR", ""}},
		AnalogJackInput4ms{{to_mm<72>(171.9), to_mm<72>(300.41), Center, "VCA CV B", ""}},
		AnalogJackInput4ms{{to_mm<72>(207.29), to_mm<72>(274.76), Center, "Audio B In", ""}},
		AnalogJackOutput4ms{{to_mm<72>(23.06), to_mm<72>(324.82), Center, "Audio A Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(90.49), to_mm<72>(324.82), Center, "Env A Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(139.91), to_mm<72>(324.88), Center, "Env B Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(207.32), to_mm<72>(324.82), Center, "Audio B Out", ""}},
		RedBlueLight{{to_mm<72>(45.84), to_mm<72>(176.26), Center, "Rise A Light", ""}},
		RedBlueLight{{to_mm<72>(70.54), to_mm<72>(176.26), Center, "Fall A Light", ""}},
		RedBlueLight{{to_mm<72>(159.84), to_mm<72>(176.26), Center, "Rise B Light", ""}},
		RedBlueLight{{to_mm<72>(184.53), to_mm<72>(176.26), Center, "Fall B Light", ""}},
		OrangeLight{{to_mm<72>(99.58), to_mm<72>(260.79), Center, "EOR Light", ""}},
		OrangeLight{{to_mm<72>(130.68), to_mm<72>(261.07), Center, "EOF Light", ""}},
		RedBlueLight{{to_mm<72>(65.92), to_mm<72>(327.45), Center, "Env A Light", ""}},
		RedBlueLight{{to_mm<72>(164.39), to_mm<72>(327.52), Center, "Env B Light", ""}},
}};

    enum class Elem {
        RiseASwitch,
        FallASwitch,
        CycleAButton,
        CycleBButton,
        RiseBSwitch,
        FallBSwitch,
        RiseASlider,
        FallASlider,
        LevelAKnob,
        LevelBKnob,
        OffsetAKnob,
        OffsetBKnob,
        RiseBSlider,
        FallBSlider,
        RiseAKnob,
        FallAKnob,
        RiseBKnob,
        FallBKnob,
        CycleGateIn,
        TimeCvAIn,
        TimeCvBIn,
        TrigAIn,
        FollowAIn,
        EorAOut,
        EofBOut,
        FollowBIn,
        TrigBIn,
        AudioAIn,
        VcaCvAIn,
        OrOut,
        VcaCvBIn,
        AudioBIn,
        AudioAOut,
        EnvAOut,
        EnvBOut,
        AudioBOut,
        RiseALight,
        FallALight,
        RiseBLight,
        FallBLight,
        EorLight,
        EofLight,
        EnvALight,
        EnvBLight,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobRise_A_Slider, 
        KnobFall_A_Slider, 
        KnobLevel_A, 
        KnobLevel_B, 
        KnobOffset_A, 
        KnobOffset_B, 
        KnobRise_B_Slider, 
        KnobFall_B_Slider, 
        KnobRise_A, 
        KnobFall_A, 
        KnobRise_B, 
        KnobFall_B, 
        NumKnobs,
    };
    
    enum {
        SwitchRise_A_Switch, 
        SwitchFall_A_Switch, 
        SwitchCycle_A, 
        SwitchCycle_B, 
        SwitchRise_B_Switch, 
        SwitchFall_B_Switch, 
        NumSwitches,
    };
    
    enum {
        InputCycle_Gate, 
        InputTime_Cv_A, 
        InputTime_Cv_B, 
        InputTrig_A, 
        InputFollow_A, 
        InputFollow_B, 
        InputTrig_B, 
        InputAudio_A_In, 
        InputVca_Cv_A, 
        InputVca_Cv_B, 
        InputAudio_B_In, 
        NumInJacks,
    };
    
    enum {
        OutputEor_A, 
        OutputEof_B, 
        OutputOr, 
        OutputAudio_A_Out, 
        OutputEnv_A_Out, 
        OutputEnv_B_Out, 
        OutputAudio_B_Out, 
        NumOutJacks,
    };
    
    enum {
        LedRise_A_Light, 
        LedFall_A_Light, 
        LedRise_B_Light, 
        LedFall_B_Light, 
        LedEor_Light, 
        LedEof_Light, 
        LedEnv_A_Light, 
        LedEnv_B_Light, 
        NumDiscreteLeds,
    };
    
};
} // namespace MetaModule
