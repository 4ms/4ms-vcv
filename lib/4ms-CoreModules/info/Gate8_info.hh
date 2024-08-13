#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct Gate8Info : ModuleInfoBase {
    static constexpr std::string_view slug{"Gate8"};
    static constexpr std::string_view description{"8 Step Gate Sequencer"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/Gate8_artwork.svg"};
	static constexpr std::string_view png_filename{"4ms/fp/Gate8.png"};

    using enum Coords;

    static constexpr std::array<Element, 12> Elements{{
		OrangeButton{{to_mm<72>(31.96), to_mm<72>(45.78), Center, "Step 1", ""}},
		OrangeButton{{to_mm<72>(83.49), to_mm<72>(45.78), Center, "Step 2", ""}},
		OrangeButton{{to_mm<72>(31.96), to_mm<72>(88.3), Center, "Step 3", ""}},
		OrangeButton{{to_mm<72>(83.49), to_mm<72>(88.3), Center, "Step 4", ""}},
		OrangeButton{{to_mm<72>(31.96), to_mm<72>(130.82), Center, "Step 5", ""}},
		OrangeButton{{to_mm<72>(83.49), to_mm<72>(130.82), Center, "Step 6", ""}},
		OrangeButton{{to_mm<72>(31.96), to_mm<72>(173.25), Center, "Step 7", ""}},
		OrangeButton{{to_mm<72>(83.49), to_mm<72>(173.34), Center, "Step 8", ""}},
		GateJackInput4ms{{to_mm<72>(32.07), to_mm<72>(262.57), Center, "Clock", ""}},
		GateJackInput4ms{{to_mm<72>(83.61), to_mm<72>(262.57), Center, "Reset", ""}},
		GateJackOutput4ms{{to_mm<72>(32.07), to_mm<72>(310.16), Center, "End", ""}},
		GateJackOutput4ms{{to_mm<72>(83.61), to_mm<72>(310.16), Center, "Out", ""}},
}};

    enum class Elem {
        Step1Button,
        Step2Button,
        Step3Button,
        Step4Button,
        Step5Button,
        Step6Button,
        Step7Button,
        Step8Button,
        ClockIn,
        ResetIn,
        EndOut,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    
    enum {
        SwitchStep_1 = 0,
        SwitchStep_2 = 1,
        SwitchStep_3 = 2,
        SwitchStep_4 = 3,
        SwitchStep_5 = 4,
        SwitchStep_6 = 5,
        SwitchStep_7 = 6,
        SwitchStep_8 = 7,
        NumSwitches,
    };
    
    enum {
        InputClock = 0,
        InputReset = 1,
        NumInJacks,
    };
    
    enum {
        OutputEnd = 0,
        OutputOut = 1,
        NumOutJacks,
    };
    
};
} // namespace MetaModule
