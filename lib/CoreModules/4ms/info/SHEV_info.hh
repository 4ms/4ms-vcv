#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct SHEVInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"SHEV"};
    static constexpr std::string_view description{"Shaped Dual EnvVCA"};
    static constexpr uint32_t width_hp = 20;
    static constexpr std::string_view svg_filename{"res/modules/SHEV_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/SHEV.png"};

    using enum Coords;

    static constexpr std::array<Element, 55> Elements{{
		OrangeButton{{to_mm<72>(21.16), to_mm<72>(41.89), Center, "Cycle A", ""}},
		Toggle3pos{{to_mm<72>(94.18), to_mm<72>(41.285), Center, "Rise A Switch", ""}, {"Fast", "Med", "Slow"}, Toggle3pos::State_t::CENTER},
		Toggle3pos{{to_mm<72>(127.06), to_mm<72>(41.275), Center, "Fall A Switch", ""}, {"Fast", "Med", "Slow"}, Toggle3pos::State_t::CENTER},
		Toggle3pos{{to_mm<72>(159.95), to_mm<72>(41.275), Center, "Rise B Switch", ""}, {"Fast", "Med", "Slow"}, Toggle3pos::State_t::CENTER},
		Toggle3pos{{to_mm<72>(192.83), to_mm<72>(41.285), Center, "Fall B Switch", ""}, {"Fast", "Med", "Slow"}, Toggle3pos::State_t::CENTER},
		OrangeButton{{to_mm<72>(267.89), to_mm<72>(41.55), Center, "Cycle B", ""}},
		Knob9mm{{to_mm<72>(23.59), to_mm<72>(86.1), Center, "Level A", ""}, 1.0f},
		Toggle3pos{{to_mm<72>(61.58), to_mm<72>(96.635), Center, "Trig Mode A", ""}, {"ASR", "AR", "Cycle"}},
		Slider25mmVertLED{{to_mm<72>(94.665), to_mm<72>(104.78), Center, "Rise A Slider", ""}, 0.5f},
		Slider25mmVertLED{{to_mm<72>(127.065), to_mm<72>(104.78), Center, "Fall A Slider", ""}, 0.5f},
		Slider25mmVertLED{{to_mm<72>(159.945), to_mm<72>(104.78), Center, "Rise B Slider", ""}, 0.5f},
		Slider25mmVertLED{{to_mm<72>(192.345), to_mm<72>(104.78), Center, "Fall B Slider", ""}, 0.5f},
		Toggle3pos{{to_mm<72>(225.43), to_mm<72>(96.635), Center, "Trig Mode B", ""}, {"ASR", "AR", "Cycle"}},
		Knob9mm{{to_mm<72>(263.62), to_mm<72>(86.05), Center, "Level B", ""}, 1.0f},
		Knob9mm{{to_mm<72>(23.67), to_mm<72>(132.68), Center, "Offset A", ""}, 0.5f},
		Knob9mm{{to_mm<72>(263.42), to_mm<72>(132.63), Center, "Offset B", ""}, 0.5f},
		Knob9mm{{to_mm<72>(23.59), to_mm<72>(179.21), Center, "Shape A", ""}, 1.0f},
		Slider25mmHorizLED{{to_mm<72>(93.36), to_mm<72>(168.515), Center, "Shape A Slider", ""}, 0.5f},
		Slider25mmHorizLED{{to_mm<72>(193.65), to_mm<72>(168.515), Center, "Shape B Slider", ""}, 0.5f},
		Knob9mm{{to_mm<72>(263.45), to_mm<72>(179.21), Center, "Shape B", ""}, 1.0f},
		Knob9mm{{to_mm<72>(68.74), to_mm<72>(206.72), Center, "Rise A", ""}, 1.0f},
		Knob9mm{{to_mm<72>(117.86), to_mm<72>(206.72), Center, "Fall A", ""}, 1.0f},
		Knob9mm{{to_mm<72>(169.15), to_mm<72>(206.72), Center, "Rise B", ""}, 1.0f},
		Knob9mm{{to_mm<72>(218.26), to_mm<72>(206.72), Center, "Fall B", ""}, 1.0f},
		GateJackInput4ms{{to_mm<72>(59.23), to_mm<72>(41.53), Center, "Trig A", ""}},
		GateJackInput4ms{{to_mm<72>(227.77), to_mm<72>(41.53), Center, "Trig B", ""}},
		AnalogJackInput4ms{{to_mm<72>(23.59), to_mm<72>(224.47), Center, "Shape CV A", ""}},
		AnalogJackInput4ms{{to_mm<72>(263.42), to_mm<72>(224.47), Center, "Shape CV B", ""}},
		AnalogJackInput4ms{{to_mm<72>(54.71), to_mm<72>(251.52), Center, "Follow A", ""}},
		AnalogJackInput4ms{{to_mm<72>(101.59), to_mm<72>(249.1), Center, "Time CV A", ""}},
		AnalogJackInput4ms{{to_mm<72>(185.42), to_mm<72>(249.1), Center, "Time CV B", ""}},
		AnalogJackInput4ms{{to_mm<72>(232.3), to_mm<72>(251.52), Center, "Follow B", ""}},
		AnalogJackInput4ms{{to_mm<72>(22.13), to_mm<72>(280.66), Center, "Audio A In", ""}},
		GateJackOutput4ms{{to_mm<72>(77.47), to_mm<72>(286.09), Center, "EOR A", ""}},
		AnalogJackOutput4ms{{to_mm<72>(121.52), to_mm<72>(286.16), Center, "Env A Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(165.48), to_mm<72>(286.16), Center, "Env B Out", ""}},
		GateJackOutput4ms{{to_mm<72>(209.54), to_mm<72>(286.09), Center, "EOF B", ""}},
		AnalogJackInput4ms{{to_mm<72>(264.88), to_mm<72>(280.66), Center, "Audio B In", ""}},
		AnalogJackOutput4ms{{to_mm<72>(22.13), to_mm<72>(323.06), Center, "Out A", ""}},
		AnalogJackInput4ms{{to_mm<72>(62.24), to_mm<72>(323.06), Center, "VCA CV A", ""}},
		AnalogJackOutput4ms{{to_mm<72>(102.87), to_mm<72>(323.06), Center, "Lin 5V A", ""}},
		AnalogJackOutput4ms{{to_mm<72>(143.5), to_mm<72>(323.06), Center, "OR", ""}},
		AnalogJackOutput4ms{{to_mm<72>(184.13), to_mm<72>(323.06), Center, "Lin 5V B", ""}},
		AnalogJackInput4ms{{to_mm<72>(224.77), to_mm<72>(323.06), Center, "VCA CV B", ""}},
		AnalogJackOutput4ms{{to_mm<72>(264.88), to_mm<72>(323.06), Center, "Out B", ""}},
		RedGreenBlueLight{{to_mm<72>(53.02), to_mm<72>(150.13), Center, "Shape A Light", ""}},
		RedGreenBlueLight{{to_mm<72>(235.49), to_mm<72>(151.05), Center, "Shape B Light", ""}},
		RedBlueLight{{to_mm<72>(88.86), to_mm<72>(196.34), Center, "Rise A Light", ""}},
		RedBlueLight{{to_mm<72>(98.07), to_mm<72>(219.09), Center, "Fall A Light", ""}},
		RedBlueLight{{to_mm<72>(188.93), to_mm<72>(196.34), Center, "Rise B Light", ""}},
		RedBlueLight{{to_mm<72>(198.15), to_mm<72>(219.09), Center, "Fall B Light", ""}},
		RedBlueLight{{to_mm<72>(133.59), to_mm<72>(263.06), Center, "Env A Light", ""}},
		RedBlueLight{{to_mm<72>(153.41), to_mm<72>(263.06), Center, "Env B Light", ""}},
		OrangeLight{{to_mm<72>(50.99), to_mm<72>(275.79), Center, "EOR A", ""}},
		OrangeLight{{to_mm<72>(236.01), to_mm<72>(275.79), Center, "EOF B Light", ""}},
}};

    enum class Elem {
        CycleAButton,
        RiseASwitch,
        FallASwitch,
        RiseBSwitch,
        FallBSwitch,
        CycleBButton,
        LevelAKnob,
        TrigModeASwitch,
        RiseASlider,
        FallASlider,
        RiseBSlider,
        FallBSlider,
        TrigModeBSwitch,
        LevelBKnob,
        OffsetAKnob,
        OffsetBKnob,
        ShapeAKnob,
        ShapeASlider,
        ShapeBSlider,
        ShapeBKnob,
        RiseAKnob,
        FallAKnob,
        RiseBKnob,
        FallBKnob,
        TrigAIn,
        TrigBIn,
        ShapeCvAIn,
        ShapeCvBIn,
        FollowAIn,
        TimeCvAIn,
        TimeCvBIn,
        FollowBIn,
        AudioAIn,
        EorAOut,
        EnvAOut,
        EnvBOut,
        EofBOut,
        AudioBIn,
        OutAOut,
        VcaCvAIn,
        Lin5VAOut,
        OrOut,
        Lin5VBOut,
        VcaCvBIn,
        OutBOut,
        ShapeALight,
        ShapeBLight,
        RiseALight,
        FallALight,
        RiseBLight,
        FallBLight,
        EnvALight,
        EnvBLight,
        EorALight,
        EofBLight,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobLevel_A, 
        KnobRise_A_Slider, 
        KnobFall_A_Slider, 
        KnobRise_B_Slider, 
        KnobFall_B_Slider, 
        KnobLevel_B, 
        KnobOffset_A, 
        KnobOffset_B, 
        KnobShape_A, 
        KnobShape_A_Slider, 
        KnobShape_B_Slider, 
        KnobShape_B, 
        KnobRise_A, 
        KnobFall_A, 
        KnobRise_B, 
        KnobFall_B, 
        NumKnobs,
    };
    
    enum {
        SwitchCycle_A, 
        SwitchRise_A_Switch, 
        SwitchFall_A_Switch, 
        SwitchRise_B_Switch, 
        SwitchFall_B_Switch, 
        SwitchCycle_B, 
        SwitchTrig_Mode_A, 
        SwitchTrig_Mode_B, 
        NumSwitches,
    };
    
    enum {
        InputTrig_A, 
        InputTrig_B, 
        InputShape_Cv_A, 
        InputShape_Cv_B, 
        InputFollow_A, 
        InputTime_Cv_A, 
        InputTime_Cv_B, 
        InputFollow_B, 
        InputAudio_A_In, 
        InputAudio_B_In, 
        InputVca_Cv_A, 
        InputVca_Cv_B, 
        NumInJacks,
    };
    
    enum {
        OutputEor_A, 
        OutputEnv_A_Out, 
        OutputEnv_B_Out, 
        OutputEof_B, 
        OutputOut_A, 
        OutputLin_5V_A, 
        OutputOr, 
        OutputLin_5V_B, 
        OutputOut_B, 
        NumOutJacks,
    };
    
    enum {
        LedShape_A_Light, 
        LedShape_B_Light, 
        LedRise_A_Light, 
        LedFall_A_Light, 
        LedRise_B_Light, 
        LedFall_B_Light, 
        LedEnv_A_Light, 
        LedEnv_B_Light, 
        LedEor_A, 
        LedEof_B_Light, 
        NumDiscreteLeds,
    };
    
};
} // namespace MetaModule
