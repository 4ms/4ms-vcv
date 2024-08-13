#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct SHInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"SH"};
    static constexpr std::string_view description{"2 Ch. Sample and Hold"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/SH_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/SH.png"};

    using enum Coords;

    static constexpr std::array<Element, 6> Elements{{
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(72.35), Center, "Samp 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(120.54), Center, "In 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(168.72), Center, "Samp 2", ""}},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(216.85), Center, "In 2", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(265.04), Center, "Out 1", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(313.23), Center, "Out 2", ""}},
}};

    enum class Elem {
        Samp1In,
        In1In,
        Samp2In,
        In2In,
        Out1Out,
        Out2Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    
    
    enum {
        InputSamp_1, 
        InputIn_1, 
        InputSamp_2, 
        InputIn_2, 
        NumInJacks,
    };
    
    enum {
        OutputOut_1, 
        OutputOut_2, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
