#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct StMixInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"StMix"};
    static constexpr std::string_view description{"Stereo Mixer"};
    static constexpr uint32_t width_hp = 18;
    static constexpr std::string_view svg_filename{"res/modules/StMix_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/StMix.png"};

    using enum Coords;

    static constexpr std::array<Element, 18> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(35.16), to_mm<72>(58.02), Center, "Level 1", ""}, 0.875f},
		Davies1900hBlackKnob{{to_mm<72>(98.12), to_mm<72>(58.02), Center, "Level 2", ""}, 0.875f},
		Davies1900hBlackKnob{{to_mm<72>(161.08), to_mm<72>(58.02), Center, "Level 3", ""}, 0.875f},
		Davies1900hBlackKnob{{to_mm<72>(224.04), to_mm<72>(58.02), Center, "Level 4", ""}, 0.875f},
		Davies1900hBlackKnob{{to_mm<72>(35.16), to_mm<72>(150.39), Center, "Pan 1", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(98.12), to_mm<72>(150.17), Center, "Pan 2", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(160.97), to_mm<72>(150.34), Center, "Pan 3", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(224.04), to_mm<72>(150.17), Center, "Pan 4", ""}, 0.5f},
		AnalogJackInput4ms{{to_mm<72>(34.78), to_mm<72>(222.7), Center, "In 1 L", ""}},
		AnalogJackInput4ms{{to_mm<72>(34.78), to_mm<72>(271.8), Center, "In 1 R", ""}},
		AnalogJackInput4ms{{to_mm<72>(97.81), to_mm<72>(222.7), Center, "In 2 L", ""}},
		AnalogJackInput4ms{{to_mm<72>(97.81), to_mm<72>(271.8), Center, "In 2 R", ""}},
		AnalogJackInput4ms{{to_mm<72>(160.85), to_mm<72>(222.7), Center, "In 3 L", ""}},
		AnalogJackInput4ms{{to_mm<72>(160.85), to_mm<72>(271.8), Center, "In 3 R", ""}},
		AnalogJackInput4ms{{to_mm<72>(223.88), to_mm<72>(222.7), Center, "In 4 L", ""}},
		AnalogJackInput4ms{{to_mm<72>(223.88), to_mm<72>(271.8), Center, "In 4 R", ""}},
		AnalogJackOutput4ms{{to_mm<72>(138.33), to_mm<72>(322.18), Center, "L", ""}},
		AnalogJackOutput4ms{{to_mm<72>(206.36), to_mm<72>(322.18), Center, "R", ""}},
}};

    enum class Elem {
        Level1Knob,
        Level2Knob,
        Level3Knob,
        Level4Knob,
        Pan1Knob,
        Pan2Knob,
        Pan3Knob,
        Pan4Knob,
        In1LIn,
        In1RIn,
        In2LIn,
        In2RIn,
        In3LIn,
        In3RIn,
        In4LIn,
        In4RIn,
        LOut,
        ROut,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobLevel_1, 
        KnobLevel_2, 
        KnobLevel_3, 
        KnobLevel_4, 
        KnobPan_1, 
        KnobPan_2, 
        KnobPan_3, 
        KnobPan_4, 
        NumKnobs,
    };
    
    
    enum {
        InputIn_1_L, 
        InputIn_1_R, 
        InputIn_2_L, 
        InputIn_2_R, 
        InputIn_3_L, 
        InputIn_3_R, 
        InputIn_4_L, 
        InputIn_4_R, 
        NumInJacks,
    };
    
    enum {
        OutputL, 
        OutputR, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
