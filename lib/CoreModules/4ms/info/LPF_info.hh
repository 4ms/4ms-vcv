#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct LPFInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"LPF"};
    static constexpr std::string_view description{"Low-Pass Filter"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/LPF_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/LPF.png"};

    using enum Coords;

    static constexpr std::array<Element, 6> Elements{{
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(46.77), Center, "Cutoff", ""}},
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(94.96), Center, "Q", ""}},
		OrangeButton{{to_mm<72>(28.8), to_mm<72>(168.66), Center, "Mode", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(216.85), Center, "CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(265.04), Center, "Input", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(313.23), Center, "Out", ""}},
}};

    enum class Elem {
        CutoffKnob,
        QKnob,
        ModeButton,
        CvIn,
        InputIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobCutoff, 
        KnobQ, 
        NumKnobs,
    };
    
    enum {
        SwitchMode, 
        NumSwitches,
    };
    
    enum {
        InputCv, 
        InputInput, 
        NumInJacks,
    };
    
    enum {
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
