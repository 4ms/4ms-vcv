#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct FreeverbInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Freeverb"};
    static constexpr std::string_view description{"Reverb Effect"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/Freeverb_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Freeverb.png"};

    using enum Coords;

    static constexpr std::array<Element, 10> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(31.96), to_mm<72>(57.97), Center, "Size", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(83.49), to_mm<72>(57.97), Center, "Feedback", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(31.96), to_mm<72>(119.21), Center, "Damp", ""}, 0.25f},
		Davies1900hBlackKnob{{to_mm<72>(83.49), to_mm<72>(119.21), Center, "Mix", ""}, 0.875f},
		AnalogJackInput4ms{{to_mm<72>(32.13), to_mm<72>(214.66), Center, "Input", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.32), to_mm<72>(214.66), Center, "Size CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(32.13), to_mm<72>(263.38), Center, "Feedback CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.32), to_mm<72>(263.38), Center, "Damp CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(32.13), to_mm<72>(312.1), Center, "Mix CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.32), to_mm<72>(312.1), Center, "Out", ""}},
}};

    enum class Elem {
        SizeKnob,
        FeedbackKnob,
        DampKnob,
        MixKnob,
        InputIn,
        SizeCvIn,
        FeedbackCvIn,
        DampCvIn,
        MixCvIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobSize, 
        KnobFeedback, 
        KnobDamp, 
        KnobMix, 
        NumKnobs,
    };
    
    
    enum {
        InputInput, 
        InputSize_Cv, 
        InputFeedback_Cv, 
        InputDamp_Cv, 
        InputMix_Cv, 
        NumInJacks,
    };
    
    enum {
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
