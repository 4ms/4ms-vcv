#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct CompInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Comp"};
    static constexpr std::string_view description{"Comparator"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/Comp_artwork.svg"};
	static constexpr std::string_view png_filename{"4ms/fp/Comp.png"};

    using enum Coords;

    static constexpr std::array<Element, 5> Elements{{
		Knob9mm{{to_mm<72>(28.93), to_mm<72>(46.53), Center, "Offset", ""}},
		Knob9mm{{to_mm<72>(28.93), to_mm<72>(94.22), Center, "CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.63), to_mm<72>(214.97), Center, "CV Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.63), to_mm<72>(264.07), Center, "Input", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.63), to_mm<72>(312.29), Center, "Out", ""}},
}};

    enum class Elem {
        OffsetKnob,
        CvKnob,
        CvJackIn,
        InputIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobOffset = 0,
        KnobCv = 1,
        NumKnobs,
    };
    
    
    enum {
        InputCv_Jack = 0,
        InputInput = 1,
        NumInJacks,
    };
    
    enum {
        OutputOut = 0,
        NumOutJacks,
    };
    
};
} // namespace MetaModule
