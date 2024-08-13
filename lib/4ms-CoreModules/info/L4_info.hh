#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct L4Info : ModuleInfoBase {
    static constexpr std::string_view slug{"L4"};
    static constexpr std::string_view description{"Listen Four"};
    static constexpr uint32_t width_hp = 10;
    static constexpr std::string_view svg_filename{"res/modules/L4_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/L4.png"};

    using enum Coords;

    static constexpr std::array<Element, 24> Elements{{
		Knob9mm{{to_mm<72>(62.33), to_mm<72>(46.17), Center, "Pan 1", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(111.89), to_mm<72>(59.09), Center, "Level 1", ""}, 0.875f},
		Knob9mm{{to_mm<72>(62.33), to_mm<72>(103.86), Center, "Pan 2", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(111.89), to_mm<72>(116.69), Center, "Level 2", ""}, 0.875f},
		Davies1900hBlackKnob{{to_mm<72>(111.89), to_mm<72>(174.29), Center, "Level 3", ""}, 0.875f},
		Davies1900hBlackKnob{{to_mm<72>(111.89), to_mm<72>(232.15), Center, "Level 4", ""}, 0.875f},
		Toggle2posHoriz{{to_mm<72>(67.265), to_mm<72>(258.19), Center, "Mod. | Line", ""}},
		Knob9mm{{to_mm<72>(23.27), to_mm<72>(318.85), Center, "Headphone Level", ""}, 0.875f},
		Davies1900hBlackKnob{{to_mm<72>(71.7), to_mm<72>(302.94), Center, "Main Level", ""}, 0.875f},
		AnalogJackInput4ms{{to_mm<72>(23.31), to_mm<72>(46.17), Center, "In 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(23.31), to_mm<72>(103.92), Center, "In 2", ""}},
		AnalogJackInput4ms{{to_mm<72>(23.31), to_mm<72>(161.52), Center, "In 3 L", ""}},
		AnalogJackInput4ms{{to_mm<72>(62.91), to_mm<72>(161.52), Center, "In 3 R", ""}},
		AnalogJackInput4ms{{to_mm<72>(23.31), to_mm<72>(219.12), Center, "In 4 L", ""}},
		AnalogJackInput4ms{{to_mm<72>(62.91), to_mm<72>(219.12), Center, "In 4 R", ""}},
		AnalogJackOutput4ms{{to_mm<72>(23.64), to_mm<72>(266.4), Center, "Headphone Out", ""}},
		AnalogJackOutput4ms{{to_mm<72>(122.02), to_mm<72>(282.83), Center, "Out Left", ""}},
		AnalogJackOutput4ms{{to_mm<72>(122.02), to_mm<72>(326.07), Center, "Out Right", ""}},
		RedGreenBlueLight{{to_mm<72>(87.38), to_mm<72>(46.17), Center, "Level 1 LED", ""}},
		RedGreenBlueLight{{to_mm<72>(87.38), to_mm<72>(105.8), Center, "Level 2 LED", ""}},
		RedGreenBlueLight{{to_mm<72>(87.38), to_mm<72>(163.03), Center, "Level 3 LED", ""}},
		RedGreenBlueLight{{to_mm<72>(87.38), to_mm<72>(221.73), Center, "Level 4 LED", ""}},
		RedGreenBlueLight{{to_mm<72>(98.07), to_mm<72>(284.23), Center, "Out L LED", ""}},
		RedGreenBlueLight{{to_mm<72>(98.07), to_mm<72>(316.53), Center, "Out R LED", ""}},
}};

    enum class Elem {
        Pan1Knob,
        Level1Knob,
        Pan2Knob,
        Level2Knob,
        Level3Knob,
        Level4Knob,
        Mod__OR_LineSwitch,
        HeadphoneLevelKnob,
        MainLevelKnob,
        In1In,
        In2In,
        In3LIn,
        In3RIn,
        In4LIn,
        In4RIn,
        HeadphoneOut,
        OutLeftOut,
        OutRightOut,
        Level1LedLight,
        Level2LedLight,
        Level3LedLight,
        Level4LedLight,
        OutLLedLight,
        OutRLedLight,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobPan_1, 
        KnobLevel_1, 
        KnobPan_2, 
        KnobLevel_2, 
        KnobLevel_3, 
        KnobLevel_4, 
        KnobHeadphone_Level, 
        KnobMain_Level, 
        NumKnobs,
    };
    
    enum {
        SwitchMod___OR__Line, 
        NumSwitches,
    };
    
    enum {
        InputIn_1, 
        InputIn_2, 
        InputIn_3_L, 
        InputIn_3_R, 
        InputIn_4_L, 
        InputIn_4_R, 
        NumInJacks,
    };
    
    enum {
        OutputHeadphone_Out, 
        OutputOut_Left, 
        OutputOut_Right, 
        NumOutJacks,
    };
    
    enum {
        LedLevel_1_Led, 
        LedLevel_2_Led, 
        LedLevel_3_Led, 
        LedLevel_4_Led, 
        LedOut_L_Led, 
        LedOut_R_Led, 
        NumDiscreteLeds,
    };
    
};
} // namespace MetaModule
