#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct QuantInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Quant"};
    static constexpr std::string_view description{"Quantizer"};
    static constexpr uint32_t width_hp = 10;
    static constexpr std::string_view svg_filename{"res/modules/Quant_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Quant.png"};

    using enum Coords;

    static constexpr std::array<Element, 9> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(72.0), to_mm<72>(57.56), Center, "Scale", ""}},
		Davies1900hBlackKnob{{to_mm<72>(37.73), to_mm<72>(119.29), Center, "Root", ""}},
		Davies1900hBlackKnob{{to_mm<72>(106.27), to_mm<72>(119.29), Center, "Transpose", ""}},
		AnalogJackInput4ms{{to_mm<72>(29.82), to_mm<72>(272.11), Center, "Input", ""}},
		AnalogJackInput4ms{{to_mm<72>(72.0), to_mm<72>(272.11), Center, "Scale CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(115.04), to_mm<72>(272.11), Center, "Root CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(29.82), to_mm<72>(313.71), Center, "Thru", ""}},
		AnalogJackOutput4ms{{to_mm<72>(72.0), to_mm<72>(313.71), Center, "Pre", ""}},
		AnalogJackOutput4ms{{to_mm<72>(115.04), to_mm<72>(313.71), Center, "Out", ""}},
}};

    enum class Elem {
        ScaleKnob,
        RootKnob,
        TransposeKnob,
        InputIn,
        ScaleCvIn,
        RootCvIn,
        ThruOut,
        PreOut,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobScale, 
        KnobRoot, 
        KnobTranspose, 
        NumKnobs,
    };
    
    
    enum {
        InputInput, 
        InputScale_Cv, 
        InputRoot_Cv, 
        NumInJacks,
    };
    
    enum {
        OutputThru, 
        OutputPre, 
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
