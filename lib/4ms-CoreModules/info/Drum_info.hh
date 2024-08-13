#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct DrumInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Drum"};
    static constexpr std::string_view description{"Drum Voice"};
    static constexpr uint32_t width_hp = 15;
    static constexpr std::string_view svg_filename{"res/modules/Drum_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Drum.png"};

    using enum Coords;

    static constexpr std::array<Element, 21> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(43.5), to_mm<72>(45.83), Center, "Pitch", ""}, 0.25f},
		Davies1900hBlackKnob{{to_mm<72>(108.0), to_mm<72>(45.83), Center, "Pitch Env", ""}, 0.25f},
		Davies1900hBlackKnob{{to_mm<72>(172.46), to_mm<72>(45.83), Center, "Pitch Amt", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(43.5), to_mm<72>(104.39), Center, "Ratio", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(108.0), to_mm<72>(104.39), Center, "FM Env", ""}, 0.25f},
		Davies1900hBlackKnob{{to_mm<72>(172.46), to_mm<72>(104.39), Center, "FM Amt", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(43.5), to_mm<72>(163.07), Center, "Tone Env", ""}, 0.25f},
		Davies1900hBlackKnob{{to_mm<72>(108.0), to_mm<72>(163.07), Center, "Noise Env", ""}, 0.0f},
		Davies1900hBlackKnob{{to_mm<72>(172.46), to_mm<72>(163.07), Center, "Noise Blend", ""}, 0.0f},
		AnalogJackInput4ms{{to_mm<72>(36.42), to_mm<72>(214.2), Center, "Trigger", ""}},
		AnalogJackInput4ms{{to_mm<72>(84.7), to_mm<72>(214.2), Center, "V/Oct", ""}},
		AnalogJackInput4ms{{to_mm<72>(132.98), to_mm<72>(214.2), Center, "P Env CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(181.26), to_mm<72>(214.2), Center, "P Amt CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(36.42), to_mm<72>(262.92), Center, "Ratio CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(84.7), to_mm<72>(262.92), Center, "FM Env CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(132.98), to_mm<72>(262.92), Center, "FM Amt CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(181.26), to_mm<72>(262.92), Center, "T Env CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(36.42), to_mm<72>(311.64), Center, "Inv Out", ""}},
		AnalogJackInput4ms{{to_mm<72>(84.7), to_mm<72>(311.64), Center, "N Env CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(132.98), to_mm<72>(311.64), Center, "N Blend CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(181.26), to_mm<72>(311.64), Center, "Out", ""}},
}};

    enum class Elem {
        PitchKnob,
        PitchEnvKnob,
        PitchAmtKnob,
        RatioKnob,
        FmEnvKnob,
        FmAmtKnob,
        ToneEnvKnob,
        NoiseEnvKnob,
        NoiseBlendKnob,
        TriggerIn,
        V_OctIn,
        PEnvCvIn,
        PAmtCvIn,
        RatioCvIn,
        FmEnvCvIn,
        FmAmtCvIn,
        TEnvCvIn,
        InvOut,
        NEnvCvIn,
        NBlendCvIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobPitch, 
        KnobPitch_Env, 
        KnobPitch_Amt, 
        KnobRatio, 
        KnobFm_Env, 
        KnobFm_Amt, 
        KnobTone_Env, 
        KnobNoise_Env, 
        KnobNoise_Blend, 
        NumKnobs,
    };
    
    
    enum {
        InputTrigger, 
        InputV_Oct, 
        InputP_Env_Cv, 
        InputP_Amt_Cv, 
        InputRatio_Cv, 
        InputFm_Env_Cv, 
        InputFm_Amt_Cv, 
        InputT_Env_Cv, 
        InputN_Env_Cv, 
        InputN_Blend_Cv, 
        NumInJacks,
    };
    
    enum {
        OutputInv_Out, 
        OutputOut, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
