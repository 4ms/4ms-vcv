#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct Mixer4Info : ModuleInfoBase {
    static constexpr std::string_view slug{"Mixer4"};
    static constexpr std::string_view description{"4 Ch. Mixer4"};
    static constexpr uint32_t width_hp = 10;
    static constexpr std::string_view svg_filename{"res/modules/Mixer4_artwork.svg"};
	static constexpr std::string_view png_filename{"4ms/fp/Mixer4.png"};

    using enum Coords;

    static constexpr std::array<Element, 10> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(37.85), to_mm<72>(46.42), Center, "Level 1", ""}},
		Davies1900hBlackKnob{{to_mm<72>(106.4), to_mm<72>(46.42), Center, "Level 2", ""}},
		Davies1900hBlackKnob{{to_mm<72>(37.85), to_mm<72>(104.62), Center, "Level 3", ""}},
		Davies1900hBlackKnob{{to_mm<72>(106.4), to_mm<72>(104.62), Center, "Level 4", ""}},
		AnalogJackInput4ms{{to_mm<72>(29.94), to_mm<72>(272.23), Center, "In 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(72.13), to_mm<72>(272.23), Center, "In 2", ""}},
		AnalogJackInput4ms{{to_mm<72>(115.17), to_mm<72>(272.23), Center, "In 3", ""}},
		AnalogJackInput4ms{{to_mm<72>(72.13), to_mm<72>(313.84), Center, "In 4", ""}},
		AnalogJackOutput4ms{{to_mm<72>(29.94), to_mm<72>(313.84), Center, "Inv", ""}},
		AnalogJackOutput4ms{{to_mm<72>(115.17), to_mm<72>(313.84), Center, "Out", ""}},
}};

    enum class Elem {
        Level1Knob,
        Level2Knob,
        Level3Knob,
        Level4Knob,
        In1In,
        In2In,
        In3In,
        In4In,
        InvOut,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobLevel_1 = 0,
        KnobLevel_2 = 1,
        KnobLevel_3 = 2,
        KnobLevel_4 = 3,
        NumKnobs,
    };
    
    
    enum {
        InputIn_1 = 0,
        InputIn_2 = 1,
        InputIn_3 = 2,
        InputIn_4 = 3,
        NumInJacks,
    };
    
    enum {
        OutputInv = 0,
        OutputOut = 1,
        NumOutJacks,
    };
    
};
} // namespace MetaModule
