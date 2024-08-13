#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct BuffMultInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"BuffMult"};
    static constexpr std::string_view description{"Buffered Multiple"};
    static constexpr uint32_t width_hp = 3;
    static constexpr std::string_view svg_filename{"res/modules/BuffMult_artwork.svg"};
	static constexpr std::string_view png_filename{"4ms/fp/BuffMult.png"};

    using enum Coords;

    static constexpr std::array<Element, 10> Elements{{
		AnalogJackInput4ms{{to_mm<72>(21.71), to_mm<72>(44.75), Center, "Input 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(21.71), to_mm<72>(182.98), Center, "Input 2", ""}},
		AnalogJackOutput4ms{{to_mm<72>(21.71), to_mm<72>(79.31), Center, "Output A", ""}},
		AnalogJackOutput4ms{{to_mm<72>(21.71), to_mm<72>(113.89), Center, "Output B", ""}},
		AnalogJackOutput4ms{{to_mm<72>(21.71), to_mm<72>(148.42), Center, "Output C", ""}},
		AnalogJackOutput4ms{{to_mm<72>(21.71), to_mm<72>(217.54), Center, "Output F", ""}},
		AnalogJackOutput4ms{{to_mm<72>(21.71), to_mm<72>(252.1), Center, "Output E", ""}},
		AnalogJackOutput4ms{{to_mm<72>(21.71), to_mm<72>(286.66), Center, "Output D", ""}},
		RedGreenBlueLight{{to_mm<72>(36.49), to_mm<72>(62.81), Center, "Input 1 LED", ""}},
		RedGreenBlueLight{{to_mm<72>(36.49), to_mm<72>(201.01), Center, "Input 2 LED", ""}},
}};

    enum class Elem {
        Input1In,
        Input2In,
        OutputAOut,
        OutputBOut,
        OutputCOut,
        OutputFOut,
        OutputEOut,
        OutputDOut,
        Input1LedLight,
        Input2LedLight,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    
    
    enum {
        InputInput_1 = 0,
        InputInput_2 = 1,
        NumInJacks,
    };
    
    enum {
        OutputOutput_A = 0,
        OutputOutput_B = 1,
        OutputOutput_C = 2,
        OutputOutput_F = 3,
        OutputOutput_E = 4,
        OutputOutput_D = 5,
        NumOutJacks,
    };
    
    enum {
        LedInput_1_Led = 0,
        LedInput_2_Led = 1,
        NumDiscreteLeds,
    };
};
} // namespace MetaModule
