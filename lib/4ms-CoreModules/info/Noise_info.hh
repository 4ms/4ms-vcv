#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct NoiseInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Noise"};
    static constexpr std::string_view description{"Noise Source"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/Noise_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Noise.png"};

    using enum Coords;

    static constexpr std::array<Element, 2> Elements{{
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(265.04), Center, "White", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(313.23), Center, "Pink", ""}},
}};

    enum class Elem {
        WhiteOut,
        PinkOut,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    
    
    
    enum {
        OutputWhite, 
        OutputPink, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
