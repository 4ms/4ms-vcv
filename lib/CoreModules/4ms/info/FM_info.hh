#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct FMInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"FM"};
    static constexpr std::string_view description{"FM Oscillator"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/FM_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/FM.png"};

    using enum Coords;

    static constexpr std::array<Element, 14> Elements{{
		Knob9mm{{to_mm<72>(31.83), to_mm<72>(40.21), Center, "Pitch", ""}, 0.5f},
		Knob9mm{{to_mm<72>(83.37), to_mm<72>(40.21), Center, "Mix", ""}, 0.5f},
		Knob9mm{{to_mm<72>(31.83), to_mm<72>(83.35), Center, "Index", ""}, 0.625f},
		Knob9mm{{to_mm<72>(83.37), to_mm<72>(83.35), Center, "Index CV", ""}, 0.5f},
		Knob9mm{{to_mm<72>(31.83), to_mm<72>(126.49), Center, "Ratio C", ""}, 0.375f},
		Knob9mm{{to_mm<72>(83.37), to_mm<72>(126.49), Center, "Ratio F", ""}, 0.0f},
		Knob9mm{{to_mm<72>(31.83), to_mm<72>(169.63), Center, "Shape", ""}, 0.0f},
		Knob9mm{{to_mm<72>(83.37), to_mm<72>(169.63), Center, "Shape CV", ""}, 0.5f},
		AnalogJackInput4ms{{to_mm<72>(31.83), to_mm<72>(214.43), Center, "V/Oct P", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.37), to_mm<72>(214.43), Center, "V/Oct S", ""}},
		AnalogJackInput4ms{{to_mm<72>(31.83), to_mm<72>(263.15), Center, "Mix CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.37), to_mm<72>(263.15), Center, "Index CV In", ""}},
		AnalogJackInput4ms{{to_mm<72>(31.83), to_mm<72>(311.88), Center, "Shape CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.37), to_mm<72>(311.88), Center, "Out", ""}},
}};

    enum class Elem {
        PitchKnob,
        MixKnob,
        IndexKnob,
        IndexCvKnob,
        RatioCKnob,
        RatioFKnob,
        ShapeKnob,
        ShapeCvKnob,
        V_OctPIn,
        V_OctSIn,
        MixCvIn,
        IndexCvIn,
        ShapeCvIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobPitch, 
        KnobMix, 
        KnobIndex, 
        KnobIndex_Cv, 
        KnobRatio_C, 
        KnobRatio_F, 
        KnobShape, 
        KnobShape_Cv, 
        NumKnobs,
    };
    
    
    enum {
        InputV_Oct_P, 
        InputV_Oct_S, 
        InputMix_Cv, 
        InputIndex_Cv_In, 
        InputShape_Cv, 
        NumInJacks,
    };
    
    enum {
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
