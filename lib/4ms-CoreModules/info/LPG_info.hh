#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct LPGInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"LPG"};
    static constexpr std::string_view description{"Low Pass Gate"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/LPG_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/LPG.png"};

    using enum Coords;

    static constexpr std::array<Element, 9> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(57.94), to_mm<72>(50.47), Center, "Level", ""}, 0.875f},
		Davies1900hBlackKnob{{to_mm<72>(57.94), to_mm<72>(111.82), Center, "Color", ""}, 0.875f},
		Davies1900hBlackKnob{{to_mm<72>(57.94), to_mm<72>(173.18), Center, "Decay", ""}, 0.875f},
		AnalogJackInput4ms{{to_mm<72>(32.13), to_mm<72>(230.48), Center, "Input", ""}},
		GateJackInput4ms{{to_mm<72>(83.32), to_mm<72>(230.48), Center, "Ping", ""}},
		AnalogJackInput4ms{{to_mm<72>(32.13), to_mm<72>(272.09), Center, "Level CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.32), to_mm<72>(272.09), Center, "Color CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(32.13), to_mm<72>(313.7), Center, "Decay CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.32), to_mm<72>(313.7), Center, "Out", ""}},
}};

    enum class Elem {
        LevelKnob,
        ColorKnob,
        DecayKnob,
        InputIn,
        PingIn,
        LevelCvIn,
        ColorCvIn,
        DecayCvIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobLevel, 
        KnobColor, 
        KnobDecay, 
        NumKnobs,
    };
    
    
    enum {
        InputInput, 
        InputPing, 
        InputLevel_Cv, 
        InputColor_Cv, 
        InputDecay_Cv, 
        NumInJacks,
    };
    
    enum {
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
