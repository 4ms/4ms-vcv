#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct QCDInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"QCD"};
    static constexpr std::string_view description{"Quad Clock Distributor"};
    static constexpr uint32_t width_hp = 22;
    static constexpr std::string_view svg_filename{"res/modules/QCD_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/QCD.png"};

    using enum Coords;

    static constexpr std::array<Element, 54> Elements{{
		WhiteMomentary7mm{{to_mm<72>(192.6), to_mm<72>(36.77), Center, "Tap Tempo", ""}},
		Knob9mm{{to_mm<72>(67.13), to_mm<72>(58.54), Center, "Gate PW Att 1", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(108.64), to_mm<72>(59.62), Center, "Gate PW 1", ""}, 0.5f},
		Toggle3posHoriz{{to_mm<72>(153.58), to_mm<72>(41.895), Center, "Inv Mode 1", ""}, {"Trig", "InvGate", "Double"}, Toggle3posHoriz::State_t::CENTER},
		Knob9mm{{to_mm<72>(153.35), to_mm<72>(81.18), Center, "Div Mult CV 1", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(202.3), to_mm<72>(84.93), Center, "Div Mult 1", ""}, 0.5f},
		Knob9mm{{to_mm<72>(67.13), to_mm<72>(134.14), Center, "Gate PW Att 2", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(108.64), to_mm<72>(135.32), Center, "Gate PW 2", ""}, 0.5f},
		Toggle3posHoriz{{to_mm<72>(153.58), to_mm<72>(110.845), Center, "Inv Mode 2", ""}, {"Trig", "InvGate", "Double"}, Toggle3posHoriz::State_t::CENTER},
		Knob9mm{{to_mm<72>(153.35), to_mm<72>(150.76), Center, "Div Mult CV 2", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(202.3), to_mm<72>(154.05), Center, "Div Mult 2", ""}, 0.5f},
		Knob9mm{{to_mm<72>(67.13), to_mm<72>(209.74), Center, "Gate PW Att 3", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(108.64), to_mm<72>(211.02), Center, "Gate PW 3", ""}, 0.5f},
		Toggle3posHoriz{{to_mm<72>(153.58), to_mm<72>(180.295), Center, "Inv Mode 3", ""}, {"Trig", "InvGate", "Double"}, Toggle3posHoriz::State_t::CENTER},
		Knob9mm{{to_mm<72>(153.35), to_mm<72>(220.44), Center, "Div Mult CV 3", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(202.3), to_mm<72>(223.17), Center, "Div Mult 3", ""}, 0.5f},
		Knob9mm{{to_mm<72>(67.13), to_mm<72>(285.34), Center, "Gate PW Att 4", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(108.64), to_mm<72>(286.72), Center, "Gate PW 4", ""}, 0.5f},
		Toggle3posHoriz{{to_mm<72>(153.58), to_mm<72>(248.795), Center, "Inv Mode 4", ""}, {"Trig", "InvGate", "Double"}, Toggle3posHoriz::State_t::CENTER},
		Knob9mm{{to_mm<72>(153.35), to_mm<72>(288.84), Center, "Div Mult CV 4", ""}, 1.0f},
		Davies1900hBlackKnob{{to_mm<72>(202.3), to_mm<72>(292.29), Center, "Div Mult 4", ""}, 0.5f},
		GateJackOutput4ms{{to_mm<72>(245.33), to_mm<72>(36.81), Center, "Tap Out", ""}},
		AnalogJackInput4ms{{to_mm<72>(23.26), to_mm<72>(54.88), Center, "Gate PW CV 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(249.46), to_mm<72>(76.58), Center, "Div Mult CV 1 Jack", ""}},
		GateJackInput4ms{{to_mm<72>(296.2), to_mm<72>(42.19), Center, "Clk In 1", ""}},
		GateJackOutput4ms{{to_mm<72>(23.26), to_mm<72>(89.44), Center, "Inv Out 1", ""}},
		GateJackInput4ms{{to_mm<72>(249.46), to_mm<72>(111.14), Center, "Reset 1", ""}},
		GateJackOutput4ms{{to_mm<72>(296.2), to_mm<72>(76.75), Center, "Out 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(23.26), to_mm<72>(130.48), Center, "Gate PW CV 2", ""}},
		AnalogJackInput4ms{{to_mm<72>(249.46), to_mm<72>(145.7), Center, "Div Mult CV 2 Jack", ""}},
		GateJackInput4ms{{to_mm<72>(296.2), to_mm<72>(117.79), Center, "Clk In 2", ""}},
		GateJackOutput4ms{{to_mm<72>(23.26), to_mm<72>(165.04), Center, "Inv Out 2", ""}},
		GateJackInput4ms{{to_mm<72>(249.46), to_mm<72>(180.26), Center, "Reset 2", ""}},
		GateJackOutput4ms{{to_mm<72>(296.2), to_mm<72>(152.35), Center, "Out 2", ""}},
		AnalogJackInput4ms{{to_mm<72>(23.26), to_mm<72>(206.08), Center, "Gate PW CV 3", ""}},
		AnalogJackInput4ms{{to_mm<72>(249.46), to_mm<72>(214.82), Center, "Div Mult CV 3 Jack", ""}},
		GateJackInput4ms{{to_mm<72>(296.2), to_mm<72>(193.39), Center, "Clk In 3", ""}},
		GateJackOutput4ms{{to_mm<72>(23.26), to_mm<72>(240.64), Center, "Inv Out 3", ""}},
		GateJackInput4ms{{to_mm<72>(249.46), to_mm<72>(249.38), Center, "Reset 3", ""}},
		GateJackOutput4ms{{to_mm<72>(296.2), to_mm<72>(227.95), Center, "Out 3", ""}},
		AnalogJackInput4ms{{to_mm<72>(23.26), to_mm<72>(281.68), Center, "Gate PW CV 4", ""}},
		AnalogJackInput4ms{{to_mm<72>(249.46), to_mm<72>(283.94), Center, "Div Mult CV 4 Jack", ""}},
		GateJackInput4ms{{to_mm<72>(296.2), to_mm<72>(268.99), Center, "Clk In 4", ""}},
		GateJackOutput4ms{{to_mm<72>(23.26), to_mm<72>(316.24), Center, "Inv Out 4", ""}},
		GateJackInput4ms{{to_mm<72>(249.46), to_mm<72>(318.5), Center, "Reset 4", ""}},
		GateJackOutput4ms{{to_mm<72>(296.2), to_mm<72>(303.55), Center, "Out 4", ""}},
		RedLight{{to_mm<72>(47.94), to_mm<72>(89.79), Center, "Inv 1 Light", ""}},
		RedLight{{to_mm<72>(273.34), to_mm<72>(76.94), Center, "Out 1 Light", ""}},
		WhiteLight{{to_mm<72>(47.94), to_mm<72>(165.39), Center, "Inv 2 Light", ""}},
		WhiteLight{{to_mm<72>(273.34), to_mm<72>(152.54), Center, "Out 2 Light", ""}},
		BlueLight{{to_mm<72>(47.56), to_mm<72>(240.85), Center, "Inv 3 Light", ""}},
		BlueLight{{to_mm<72>(273.34), to_mm<72>(228.14), Center, "Out 3 Light", ""}},
		GreenLight{{to_mm<72>(47.75), to_mm<72>(316.59), Center, "Inv 4 Light", ""}},
		GreenLight{{to_mm<72>(273.34), to_mm<72>(303.74), Center, "Out 4 Light", ""}},
}};

    enum class Elem {
        TapTempoButton,
        GatePwAtt1Knob,
        GatePw1Knob,
        InvMode1Switch,
        DivMultCv1Knob,
        DivMult1Knob,
        GatePwAtt2Knob,
        GatePw2Knob,
        InvMode2Switch,
        DivMultCv2Knob,
        DivMult2Knob,
        GatePwAtt3Knob,
        GatePw3Knob,
        InvMode3Switch,
        DivMultCv3Knob,
        DivMult3Knob,
        GatePwAtt4Knob,
        GatePw4Knob,
        InvMode4Switch,
        DivMultCv4Knob,
        DivMult4Knob,
        TapOut,
        GatePwCv1In,
        DivMultCv1JackIn,
        ClkIn1In,
        InvOut1Out,
        Reset1In,
        Out1Out,
        GatePwCv2In,
        DivMultCv2JackIn,
        ClkIn2In,
        InvOut2Out,
        Reset2In,
        Out2Out,
        GatePwCv3In,
        DivMultCv3JackIn,
        ClkIn3In,
        InvOut3Out,
        Reset3In,
        Out3Out,
        GatePwCv4In,
        DivMultCv4JackIn,
        ClkIn4In,
        InvOut4Out,
        Reset4In,
        Out4Out,
        Inv1Light,
        Out1Light,
        Inv2Light,
        Out2Light,
        Inv3Light,
        Out3Light,
        Inv4Light,
        Out4Light,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobGate_Pw_Att_1, 
        KnobGate_Pw_1, 
        KnobDiv_Mult_Cv_1, 
        KnobDiv_Mult_1, 
        KnobGate_Pw_Att_2, 
        KnobGate_Pw_2, 
        KnobDiv_Mult_Cv_2, 
        KnobDiv_Mult_2, 
        KnobGate_Pw_Att_3, 
        KnobGate_Pw_3, 
        KnobDiv_Mult_Cv_3, 
        KnobDiv_Mult_3, 
        KnobGate_Pw_Att_4, 
        KnobGate_Pw_4, 
        KnobDiv_Mult_Cv_4, 
        KnobDiv_Mult_4, 
        NumKnobs,
    };
    
    enum {
        SwitchTap_Tempo, 
        SwitchInv_Mode_1, 
        SwitchInv_Mode_2, 
        SwitchInv_Mode_3, 
        SwitchInv_Mode_4, 
        NumSwitches,
    };
    
    enum {
        InputGate_Pw_Cv_1, 
        InputDiv_Mult_Cv_1_Jack, 
        InputClk_In_1, 
        InputReset_1, 
        InputGate_Pw_Cv_2, 
        InputDiv_Mult_Cv_2_Jack, 
        InputClk_In_2, 
        InputReset_2, 
        InputGate_Pw_Cv_3, 
        InputDiv_Mult_Cv_3_Jack, 
        InputClk_In_3, 
        InputReset_3, 
        InputGate_Pw_Cv_4, 
        InputDiv_Mult_Cv_4_Jack, 
        InputClk_In_4, 
        InputReset_4, 
        NumInJacks,
    };
    
    enum {
        OutputTap_Out, 
        OutputInv_Out_1, 
        OutputOut_1, 
        OutputInv_Out_2, 
        OutputOut_2, 
        OutputInv_Out_3, 
        OutputOut_3, 
        OutputInv_Out_4, 
        OutputOut_4, 
        NumOutJacks,
    };
    
    enum {
        LedInv_1_Light, 
        LedOut_1_Light, 
        LedInv_2_Light, 
        LedOut_2_Light, 
        LedInv_3_Light, 
        LedOut_3_Light, 
        LedInv_4_Light, 
        LedOut_4_Light, 
        NumDiscreteLeds,
    };
    
};
} // namespace MetaModule
