#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct InfOscInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"InfOsc"};
    static constexpr std::string_view description{"Infinite Oscillator"};
    static constexpr uint32_t width_hp = 15;
    static constexpr std::string_view svg_filename{"res/modules/InfOsc_artwork.svg"};
	static constexpr std::string_view png_filename{"4ms/fp/InfOsc.png"};

    using enum Coords;

    static constexpr std::array<Element, 20> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(43.62), to_mm<72>(46.55), Center, "Track", ""}},
		Davies1900hBlackKnob{{to_mm<72>(108.12), to_mm<72>(46.55), Center, "Abs", ""}},
		Davies1900hBlackKnob{{to_mm<72>(172.62), to_mm<72>(46.55), Center, "Range", ""}},
		Davies1900hBlackKnob{{to_mm<72>(43.62), to_mm<72>(105.5), Center, "FM", ""}},
		DaviesLargeKnob{{to_mm<72>(108.13), to_mm<72>(117.06), Center, "Pitch", ""}},
		Davies1900hBlackKnob{{to_mm<72>(172.62), to_mm<72>(105.5), Center, "Phase", ""}},
		Davies1900hBlackKnob{{to_mm<72>(43.3), to_mm<72>(164.67), Center, "Modshape", ""}},
		Davies1900hBlackKnob{{to_mm<72>(172.52), to_mm<72>(164.62), Center, "Wave", ""}},
		AnalogJackInput4ms{{to_mm<72>(35.96), to_mm<72>(214.54), Center, "PM CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.96), to_mm<72>(214.54), Center, "Scan CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(131.96), to_mm<72>(214.54), Center, "Add CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(179.96), to_mm<72>(214.54), Center, "Reset CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(35.96), to_mm<72>(263.64), Center, "Mod CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.96), to_mm<72>(263.64), Center, "Abs CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(131.96), to_mm<72>(263.64), Center, "Lin CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(179.96), to_mm<72>(263.64), Center, "Phase CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(35.96), to_mm<72>(311.23), Center, "Pitch CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(83.96), to_mm<72>(311.23), Center, "Range CV", ""}},
		AnalogJackInput4ms{{to_mm<72>(131.96), to_mm<72>(311.23), Center, "FM CV", ""}},
		AnalogJackOutput4ms{{to_mm<72>(179.96), to_mm<72>(311.23), Center, "Out", ""}},
}};

    enum class Elem {
        TrackKnob,
        AbsKnob,
        RangeKnob,
        FmKnob,
        PitchKnob,
        PhaseKnob,
        ModshapeKnob,
        WaveKnob,
        PmCvIn,
        ScanCvIn,
        AddCvIn,
        ResetCvIn,
        ModCvIn,
        AbsCvIn,
        LinCvIn,
        PhaseCvIn,
        PitchCvIn,
        RangeCvIn,
        FmCvIn,
        Out,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobTrack = 0,
        KnobAbs = 1,
        KnobRange = 2,
        KnobFm = 3,
        KnobPitch = 4,
        KnobPhase = 5,
        KnobModshape = 6,
        KnobWave = 7,
        NumKnobs,
    };
    
    
    enum {
        InputPm_Cv = 0,
        InputScan_Cv = 1,
        InputAdd_Cv = 2,
        InputReset_Cv = 3,
        InputMod_Cv = 4,
        InputAbs_Cv = 5,
        InputLin_Cv = 6,
        InputPhase_Cv = 7,
        InputPitch_Cv = 8,
        InputRange_Cv = 9,
        InputFm_Cv = 10,
        NumInJacks,
    };
    
    enum {
        OutputOut = 0,
        NumOutJacks,
    };
    
};
} // namespace MetaModule
