#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct SISMInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"SISM"};
    static constexpr std::string_view description{"Shifting Inverting Signal Mingler"};
    static constexpr uint32_t width_hp = 12;
    static constexpr std::string_view svg_filename{"res/modules/SISM_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/SISM.png"};

    using enum Coords;

    static constexpr std::array<Element, 34> Elements{{
		Knob9mm{{to_mm<72>(64.25), to_mm<72>(46.64), Center, "Scale 1", ""}, 1.0f},
		Knob9mm{{to_mm<72>(107.45), to_mm<72>(46.64), Center, "Shift 1", ""}, 0.5f},
		Knob9mm{{to_mm<72>(64.25), to_mm<72>(111.44), Center, "Scale 2", ""}, 1.0f},
		Knob9mm{{to_mm<72>(107.45), to_mm<72>(111.44), Center, "Shift 2", ""}, 0.5f},
		Knob9mm{{to_mm<72>(64.25), to_mm<72>(176.24), Center, "Scale 3", ""}, 1.0f},
		Knob9mm{{to_mm<72>(107.45), to_mm<72>(176.24), Center, "Shift 3", ""}, 0.5f},
		Knob9mm{{to_mm<72>(64.25), to_mm<72>(241.04), Center, "Scale 4", ""}, 1.0f},
		Knob9mm{{to_mm<72>(107.45), to_mm<72>(241.04), Center, "Shift 4", ""}, 0.5f},
		AnalogJackInput4ms{{to_mm<72>(21.77), to_mm<72>(52.84), Center, "In 1", ""}},
		AnalogJackOutput4ms{{to_mm<72>(150.52), to_mm<72>(52.9), Center, "Out 1", ""}},
		AnalogJackInput4ms{{to_mm<72>(21.77), to_mm<72>(117.64), Center, "In 2", ""}},
		AnalogJackOutput4ms{{to_mm<72>(150.52), to_mm<72>(117.7), Center, "Out 2", ""}},
		AnalogJackInput4ms{{to_mm<72>(21.77), to_mm<72>(182.44), Center, "In 3", ""}},
		AnalogJackOutput4ms{{to_mm<72>(150.52), to_mm<72>(182.5), Center, "Out 3", ""}},
		AnalogJackInput4ms{{to_mm<72>(21.77), to_mm<72>(247.24), Center, "In 4", ""}},
		AnalogJackOutput4ms{{to_mm<72>(150.52), to_mm<72>(247.3), Center, "Out 4", ""}},
		AnalogJackOutput4ms{{to_mm<72>(21.1), to_mm<72>(312.16), Center, "+ Slice", ""}},
		AnalogJackOutput4ms{{to_mm<72>(64.25), to_mm<72>(312.16), Center, "- Slice", ""}},
		AnalogJackOutput4ms{{to_mm<72>(107.45), to_mm<72>(312.16), Center, "Mix (SW)", ""}},
		AnalogJackOutput4ms{{to_mm<72>(150.7), to_mm<72>(312.16), Center, "Mix", ""}},
		RedLight{{to_mm<72>(146.69), to_mm<72>(29.5), Center, "LED - 1", ""}},
		BlueLight{{to_mm<72>(158.21), to_mm<72>(29.5), Center, "LED + 1", ""}},
		RedLight{{to_mm<72>(144.99), to_mm<72>(93.58), Center, "LED - 2", ""}},
		BlueLight{{to_mm<72>(156.5), to_mm<72>(93.58), Center, "LED + 2", ""}},
		RedLight{{to_mm<72>(145.02), to_mm<72>(158.45), Center, "LED - 3", ""}},
		BlueLight{{to_mm<72>(156.54), to_mm<72>(158.45), Center, "LED + 3", ""}},
		RedLight{{to_mm<72>(145.11), to_mm<72>(223.33), Center, "LED - 4", ""}},
		BlueLight{{to_mm<72>(156.63), to_mm<72>(223.33), Center, "LED + 4", ""}},
		BlueLight{{to_mm<72>(20.92), to_mm<72>(289.03), Center, "LED + Slice", ""}},
		RedLight{{to_mm<72>(64.26), to_mm<72>(288.6), Center, "LED - Slice", ""}},
		RedLight{{to_mm<72>(102.32), to_mm<72>(289.18), Center, "LED - Mix (SW)", ""}},
		BlueLight{{to_mm<72>(113.66), to_mm<72>(289.18), Center, "LED + Mix (SW)", ""}},
		RedLight{{to_mm<72>(145.66), to_mm<72>(289.03), Center, "LED - Mix", ""}},
		BlueLight{{to_mm<72>(157.0), to_mm<72>(289.03), Center, "LED + Mix", ""}},
}};

    enum class Elem {
        Scale1Knob,
        Shift1Knob,
        Scale2Knob,
        Shift2Knob,
        Scale3Knob,
        Shift3Knob,
        Scale4Knob,
        Shift4Knob,
        In1In,
        Out1Out,
        In2In,
        Out2Out,
        In3In,
        Out3Out,
        In4In,
        Out4Out,
        PSliceOut,
        NSliceOut,
        Mix_Sw_Out,
        MixOut,
        LedN1Light,
        LedP1Light,
        LedN2Light,
        LedP2Light,
        LedN3Light,
        LedP3Light,
        LedN4Light,
        LedP4Light,
        LedPSliceLight,
        LedNSliceLight,
        LedNMix_Sw_Light,
        LedPMix_Sw_Light,
        LedNMixLight,
        LedPMixLight,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobScale_1, 
        KnobShift_1, 
        KnobScale_2, 
        KnobShift_2, 
        KnobScale_3, 
        KnobShift_3, 
        KnobScale_4, 
        KnobShift_4, 
        NumKnobs,
    };
    
    
    enum {
        InputIn_1, 
        InputIn_2, 
        InputIn_3, 
        InputIn_4, 
        NumInJacks,
    };
    
    enum {
        OutputOut_1, 
        OutputOut_2, 
        OutputOut_3, 
        OutputOut_4, 
        OutputP_Slice, 
        OutputN_Slice, 
        OutputMix__Sw_, 
        OutputMix, 
        NumOutJacks,
    };
    
    enum {
        LedLed_N_1, 
        LedLed_P_1, 
        LedLed_N_2, 
        LedLed_P_2, 
        LedLed_N_3, 
        LedLed_P_3, 
        LedLed_N_4, 
        LedLed_P_4, 
        LedLed_P_Slice, 
        LedLed_N_Slice, 
        LedLed_N_Mix__Sw_, 
        LedLed_P_Mix__Sw_, 
        LedLed_N_Mix, 
        LedLed_P_Mix, 
        NumDiscreteLeds,
    };
    
};
} // namespace MetaModule
