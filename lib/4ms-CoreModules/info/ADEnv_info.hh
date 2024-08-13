#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct ADEnvInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"ADEnv"};
    static constexpr std::string_view description{"Attack Decay Envelope"};
    static constexpr uint32_t width_hp = 8;
    static constexpr std::string_view svg_filename{"res/modules/ADEnv_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/ADEnv.png"};

    using enum Coords;

    static constexpr std::array<Element, 12> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(31.73), to_mm<72>(47.84), Center, "Attack", ""}},
		Davies1900hBlackKnob{{to_mm<72>(83.37), to_mm<72>(47.84), Center, "Decay", ""}},
		Davies1900hBlackKnob{{to_mm<72>(31.83), to_mm<72>(104.81), Center, "A Shape", ""}},
		Davies1900hBlackKnob{{to_mm<72>(83.37), to_mm<72>(104.81), Center, "D Shape", ""}},
		GateJackInput4ms{{to_mm<72>(31.97), to_mm<72>(167.24), Center, "Gate", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.51), to_mm<72>(167.24), Center, "Decay CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(31.97), to_mm<72>(214.44), Center, "Attack CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.51), to_mm<72>(214.44), Center, "D Shape CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(31.97), to_mm<72>(263.51), Center, "A Shape CV", ""}},
		GateJackOutput4ms{{to_mm<72>(83.51), to_mm<72>(263.54), Center, "EOD", ""}},
		GateJackOutput4ms{{to_mm<72>(31.97), to_mm<72>(311.1), Center, "EOA", ""}},
		AnalogJackOutput4ms{{to_mm<72>(83.51), to_mm<72>(311.13), Center, "Out", ""}},
}};

    enum class Elem {
        AttackKnob,
        DecayKnob,
        AShapeKnob,
        DShapeKnob,
        GateIn,
        DecayCvIn,
        AttackCvIn,
        DShapeCvIn,
        AShapeCvIn,
        EodOut,
        EoaOut,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobAttack, 
        KnobDecay, 
        KnobA_Shape, 
        KnobD_Shape, 
        NumKnobs,
    };
    
    
    enum {
        InputGate, 
        InputDecay_Cv, 
        InputAttack_Cv, 
        InputD_Shape_Cv, 
        InputA_Shape_Cv, 
        NumInJacks,
    };
    
    enum {
        OutputEod, 
        OutputEoa, 
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
