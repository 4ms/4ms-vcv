#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct Switch41Info : ModuleInfoBase {
    static constexpr std::string_view slug{"Switch41"};
    static constexpr std::string_view description{"4 to 1 Switch"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/Switch41_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Switch41.png"};

    using enum Coords;

    static constexpr std::array<Element, 8> Elements{{
		AnalogJackInput4ms{{to_mm<72>(31.76), to_mm<72>(167.04), Center, "In 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.3), to_mm<72>(167.04), Center, "In 2", ""}},
		AnalogJackInput4ms{{to_mm<72>(31.76), to_mm<72>(214.43), Center, "In 3", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.3), to_mm<72>(214.43), Center, "In 4", ""}},
		AnalogJackInput4ms{{to_mm<72>(31.76), to_mm<72>(263.15), Center, "Clock", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.3), to_mm<72>(263.15), Center, "CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(31.76), to_mm<72>(311.88), Center, "Reset", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.3), to_mm<72>(311.88), Center, "Out", ""}},
}};

    enum class Elem {
        In1In,
        In2In,
        In3In,
        In4In,
        ClockIn,
        CvIn,
        ResetIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    
    
    enum {
        InputIn_1, 
        InputIn_2, 
        InputIn_3, 
        InputIn_4, 
        InputClock, 
        InputCv, 
        InputReset, 
        NumInJacks,
    };
    
    enum {
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
