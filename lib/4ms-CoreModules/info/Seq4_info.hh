#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct Seq4Info : ModuleInfoBase {
    static constexpr std::string_view slug{"Seq4"};
    static constexpr std::string_view description{"4 Step Sequencer"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/Seq4_artwork.svg"};
	static constexpr std::string_view png_filename{"4ms/fp/Seq4.png"};

    using enum Coords;

    static constexpr std::array<Element, 6> Elements{{
		Knob9mm{{to_mm<72>(28.93), to_mm<72>(46.53), Center, "1", ""}},
		Knob9mm{{to_mm<72>(28.93), to_mm<72>(94.22), Center, "2", ""}},
		Knob9mm{{to_mm<72>(28.93), to_mm<72>(142.91), Center, "3", ""}},
		Knob9mm{{to_mm<72>(28.93), to_mm<72>(191.1), Center, "4", ""}},
		AnalogJackInput4ms{{to_mm<72>(29.28), to_mm<72>(263.82), Center, "Clock", ""}},
		AnalogJackOutput4ms{{to_mm<72>(29.28), to_mm<72>(312.04), Center, "Out", ""}},
}};

    enum class Elem {
        _1Knob,
        _2Knob,
        _3Knob,
        _4Knob,
        ClockIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        Knob_1 = 0,
        Knob_2 = 1,
        Knob_3 = 2,
        Knob_4 = 3,
        NumKnobs,
    };
    
    
    enum {
        InputClock = 0,
        NumInJacks,
    };
    
    enum {
        OutputOut = 0,
        NumOutJacks,
    };
    
};
} // namespace MetaModule
