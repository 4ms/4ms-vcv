#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct Gate32Info : ModuleInfoBase {
    static constexpr std::string_view slug{"Gate32"};
    static constexpr std::string_view description{"32 Step Gate Sequencer"};
    static constexpr uint32_t width_hp = 17;
    static constexpr std::string_view svg_filename{"res/modules/Gate32_artwork.svg"};
	static constexpr std::string_view png_filename{"4ms/fp/Gate32.png"};

    using enum Coords;

    static constexpr std::array<Element, 48> Elements{{
		Knob9mm{{to_mm<72>(64.34), to_mm<72>(46.59), Center, "Tempo", ""}},
		Knob9mm{{to_mm<72>(102.55), to_mm<72>(46.59), Center, "Delay", ""}},
		Knob9mm{{to_mm<72>(140.76), to_mm<72>(46.59), Center, "Gate", ""}},
		Knob9mm{{to_mm<72>(178.96), to_mm<72>(46.59), Center, "Repeat", ""}},
		WhiteMomentary7mm{{to_mm<72>(22.96), to_mm<72>(46.59), Center, "Start/Stop", ""}},
		WhiteMomentary7mm{{to_mm<72>(221.38), to_mm<72>(46.59), Center, "Select", ""}},
		OrangeButton{{to_mm<72>(22.96), to_mm<72>(92.5), Center, "Step 1", ""}},
		OrangeButton{{to_mm<72>(51.3), to_mm<72>(92.5), Center, "Step 2", ""}},
		OrangeButton{{to_mm<72>(79.65), to_mm<72>(92.5), Center, "Step 3", ""}},
		OrangeButton{{to_mm<72>(108.0), to_mm<72>(92.5), Center, "Step 4", ""}},
		OrangeButton{{to_mm<72>(136.34), to_mm<72>(92.5), Center, "Step 5", ""}},
		OrangeButton{{to_mm<72>(164.69), to_mm<72>(92.5), Center, "Step 6", ""}},
		OrangeButton{{to_mm<72>(193.04), to_mm<72>(92.5), Center, "Step 7", ""}},
		OrangeButton{{to_mm<72>(221.38), to_mm<72>(92.5), Center, "Step 8", ""}},
		OrangeButton{{to_mm<72>(22.96), to_mm<72>(133.27), Center, "Step 9", ""}},
		OrangeButton{{to_mm<72>(51.3), to_mm<72>(133.27), Center, "Step 10", ""}},
		OrangeButton{{to_mm<72>(79.65), to_mm<72>(133.27), Center, "Step 11", ""}},
		OrangeButton{{to_mm<72>(108.0), to_mm<72>(133.27), Center, "Step 12", ""}},
		OrangeButton{{to_mm<72>(136.34), to_mm<72>(133.27), Center, "Step 13", ""}},
		OrangeButton{{to_mm<72>(164.69), to_mm<72>(133.27), Center, "Step 14", ""}},
		OrangeButton{{to_mm<72>(193.04), to_mm<72>(133.27), Center, "Step 15", ""}},
		OrangeButton{{to_mm<72>(221.38), to_mm<72>(133.27), Center, "Step 16", ""}},
		OrangeButton{{to_mm<72>(22.96), to_mm<72>(174.05), Center, "Step 17", ""}},
		OrangeButton{{to_mm<72>(51.3), to_mm<72>(174.05), Center, "Step 18", ""}},
		OrangeButton{{to_mm<72>(79.65), to_mm<72>(174.05), Center, "Step 19", ""}},
		OrangeButton{{to_mm<72>(108.0), to_mm<72>(174.05), Center, "Step 20", ""}},
		OrangeButton{{to_mm<72>(136.34), to_mm<72>(174.05), Center, "Step 21", ""}},
		OrangeButton{{to_mm<72>(164.69), to_mm<72>(174.05), Center, "Step 22", ""}},
		OrangeButton{{to_mm<72>(193.04), to_mm<72>(174.05), Center, "Step 23", ""}},
		OrangeButton{{to_mm<72>(221.38), to_mm<72>(174.05), Center, "Step 24", ""}},
		OrangeButton{{to_mm<72>(22.96), to_mm<72>(214.83), Center, "Step 25", ""}},
		OrangeButton{{to_mm<72>(51.3), to_mm<72>(214.83), Center, "Step 26", ""}},
		OrangeButton{{to_mm<72>(79.65), to_mm<72>(214.83), Center, "Step 27", ""}},
		OrangeButton{{to_mm<72>(108.0), to_mm<72>(214.83), Center, "Step 28", ""}},
		OrangeButton{{to_mm<72>(136.34), to_mm<72>(214.83), Center, "Step 29", ""}},
		OrangeButton{{to_mm<72>(164.69), to_mm<72>(214.83), Center, "Step 30", ""}},
		OrangeButton{{to_mm<72>(193.04), to_mm<72>(214.83), Center, "Step 31", ""}},
		OrangeButton{{to_mm<72>(221.38), to_mm<72>(214.83), Center, "Step 32", ""}},
		GateJackInput4ms{{to_mm<72>(31.99), to_mm<72>(263.37), Center, "Clock", ""}},
		GateJackInput4ms{{to_mm<72>(77.19), to_mm<72>(263.37), Center, "Start/Reset", ""}},
		GateJackInput4ms{{to_mm<72>(122.38), to_mm<72>(263.37), Center, "Stop", ""}},
		AnalogJackInput4ms{{to_mm<72>(167.58), to_mm<72>(263.37), Center, "Gate CV Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(212.77), to_mm<72>(263.37), Center, "Repeat CV Jack", ""}},
		GateJackOutput4ms{{to_mm<72>(31.99), to_mm<72>(310.96), Center, "Clock Out", ""}},
		GateJackOutput4ms{{to_mm<72>(77.19), to_mm<72>(310.96), Center, "Start/End", ""}},
		GateJackOutput4ms{{to_mm<72>(122.25), to_mm<72>(310.96), Center, "Trigger", ""}},
		GateJackOutput4ms{{to_mm<72>(167.58), to_mm<72>(310.96), Center, "Inv", ""}},
		GateJackOutput4ms{{to_mm<72>(212.77), to_mm<72>(310.96), Center, "Out", ""}},
}};

    enum class Elem {
        TempoKnob,
        DelayKnob,
        GateKnob,
        RepeatKnob,
        Start_StopButton,
        SelectButton,
        Step1Button,
        Step2Button,
        Step3Button,
        Step4Button,
        Step5Button,
        Step6Button,
        Step7Button,
        Step8Button,
        Step9Button,
        Step10Button,
        Step11Button,
        Step12Button,
        Step13Button,
        Step14Button,
        Step15Button,
        Step16Button,
        Step17Button,
        Step18Button,
        Step19Button,
        Step20Button,
        Step21Button,
        Step22Button,
        Step23Button,
        Step24Button,
        Step25Button,
        Step26Button,
        Step27Button,
        Step28Button,
        Step29Button,
        Step30Button,
        Step31Button,
        Step32Button,
        ClockIn,
        Start_ResetIn,
        StopIn,
        GateCvJackIn,
        RepeatCvJackIn,
        ClockOut,
        Start_EndOut,
        TriggerOut,
        InvOut,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobTempo = 0,
        KnobDelay = 1,
        KnobGate = 2,
        KnobRepeat = 3,
        NumKnobs,
    };
    
    enum {
        SwitchStart_Stop = 0,
        SwitchSelect = 1,
        SwitchStep_1 = 2,
        SwitchStep_2 = 3,
        SwitchStep_3 = 4,
        SwitchStep_4 = 5,
        SwitchStep_5 = 6,
        SwitchStep_6 = 7,
        SwitchStep_7 = 8,
        SwitchStep_8 = 9,
        SwitchStep_9 = 10,
        SwitchStep_10 = 11,
        SwitchStep_11 = 12,
        SwitchStep_12 = 13,
        SwitchStep_13 = 14,
        SwitchStep_14 = 15,
        SwitchStep_15 = 16,
        SwitchStep_16 = 17,
        SwitchStep_17 = 18,
        SwitchStep_18 = 19,
        SwitchStep_19 = 20,
        SwitchStep_20 = 21,
        SwitchStep_21 = 22,
        SwitchStep_22 = 23,
        SwitchStep_23 = 24,
        SwitchStep_24 = 25,
        SwitchStep_25 = 26,
        SwitchStep_26 = 27,
        SwitchStep_27 = 28,
        SwitchStep_28 = 29,
        SwitchStep_29 = 30,
        SwitchStep_30 = 31,
        SwitchStep_31 = 32,
        SwitchStep_32 = 33,
        NumSwitches,
    };
    
    enum {
        InputClock = 0,
        InputStart_Reset = 1,
        InputStop = 2,
        InputGate_Cv_Jack = 3,
        InputRepeat_Cv_Jack = 4,
        NumInJacks,
    };
    
    enum {
        OutputClock_Out = 0,
        OutputStart_End = 1,
        OutputTrigger = 2,
        OutputInv = 3,
        OutputOut = 4,
        NumOutJacks,
    };
    
};
} // namespace MetaModule
