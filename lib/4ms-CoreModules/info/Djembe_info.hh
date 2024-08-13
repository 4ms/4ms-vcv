#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct DjembeInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Djembe"};
    static constexpr std::string_view description{"Djembe Drum Voice"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/Djembe_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Djembe.png"};

    using enum Coords;

    static constexpr std::array<Element, 10> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(31.83), to_mm<72>(57.85), Center, "Pitch", ""}, 0.25f},
		Davies1900hBlackKnob{{to_mm<72>(83.37), to_mm<72>(57.85), Center, "Sharpness", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(31.83), to_mm<72>(119.09), Center, "Hit", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(83.37), to_mm<72>(119.09), Center, "Strike Amt", ""}, 0.5f},
		AnalogJackInput4ms{{to_mm<72>(32.0), to_mm<72>(214.53), Center, "Pitch CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.2), to_mm<72>(214.53), Center, "Sharp CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(32.0), to_mm<72>(263.25), Center, "Hit CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.2), to_mm<72>(263.25), Center, "Strike CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(32.0), to_mm<72>(311.98), Center, "Trigger", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.2), to_mm<72>(311.98), Center, "Out", ""}},
}};

    enum class Elem {
        PitchKnob,
        SharpnessKnob,
        HitKnob,
        StrikeAmtKnob,
        PitchCvIn,
        SharpCvIn,
        HitCvIn,
        StrikeCvIn,
        TriggerIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobPitch, 
        KnobSharpness, 
        KnobHit, 
        KnobStrike_Amt, 
        NumKnobs,
    };
    
    
    enum {
        InputPitch_Cv, 
        InputSharp_Cv, 
        InputHit_Cv, 
        InputStrike_Cv, 
        InputTrigger, 
        NumInJacks,
    };
    
    enum {
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
