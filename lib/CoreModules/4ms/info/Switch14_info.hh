#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct Switch14Info : ModuleInfoBase {
    static constexpr std::string_view slug{"Switch14"};
    static constexpr std::string_view description{"1 to 4 Switch"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/Switch14_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Switch14.png"};

    using enum Coords;

    static constexpr std::array<Element, 8> Elements{{
		AnalogJackInput4ms{{to_mm<72>(31.76), to_mm<72>(167.04), Center, "In", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.3), to_mm<72>(167.04), Center, "Reset", ""}},
		AnalogJackInput4ms{{to_mm<72>(31.76), to_mm<72>(214.43), Center, "Clock", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.3), to_mm<72>(214.43), Center, "CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(31.76), to_mm<72>(263.15), Center, "Out 1", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.3), to_mm<72>(263.15), Center, "Out 2", ""}},
		AnalogJackOutput4ms{{to_mm<72>(31.76), to_mm<72>(311.88), Center, "Out 3", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.3), to_mm<72>(311.88), Center, "Out 4", ""}},
}};

    enum class Elem {
        In,
        ResetIn,
        ClockIn,
        CvIn,
        Out1Out,
        Out2Out,
        Out3Out,
        Out4Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    
    
    enum {
        InputIn, 
        InputReset, 
        InputClock, 
        InputCv, 
        NumInJacks,
    };
    
    enum {
        OutputOut_1, 
        OutputOut_2, 
        OutputOut_3, 
        OutputOut_4, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
