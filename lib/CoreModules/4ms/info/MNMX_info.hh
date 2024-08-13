#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct MNMXInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"MNMX"};
    static constexpr std::string_view description{"Min Max Logic"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/MNMX_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/MNMX.png"};

    using enum Coords;

    static constexpr std::array<Element, 4> Elements{{
		AnalogJackInput4ms{{to_mm<72>(28.84), to_mm<72>(168.72), Center, "In A", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.84), to_mm<72>(216.89), Center, "In B", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.84), to_mm<72>(265.06), Center, "Min", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.84), to_mm<72>(313.23), Center, "Max", ""}},
}};

    enum class Elem {
        InAIn,
        InBIn,
        MinOut,
        MaxOut,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    
    
    enum {
        InputIn_A, 
        InputIn_B, 
        NumInJacks,
    };
    
    enum {
        OutputMin, 
        OutputMax, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
