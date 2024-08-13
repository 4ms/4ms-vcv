#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct BPFInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"BPF"};
    static constexpr std::string_view description{"Band-Pass Filter"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/BPF_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/BPF.png"};

    using enum Coords;

    static constexpr std::array<Element, 6> Elements{{
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(46.77), Center, "Cutoff", ""}, 0.0f},
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(94.96), Center, "Q", ""}, 0.0f},
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
