#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct STSInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"STS"};
    static constexpr std::string_view description{"Stereo Triggered Sampler"};
    static constexpr uint32_t width_hp = 20;
    static constexpr std::string_view svg_filename{"res/modules/STS_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/STS.png"};

    using enum Coords;

    static constexpr std::array<Element, 41> Elements{{
		Davies1900hBlackKnob{{to_mm<72>(37.52), to_mm<72>(54.34), Center, "Pitch L", ""}, 0.5f},
		WhiteMomentary7mm{{to_mm<72>(81.91), to_mm<72>(49.62), Center, "Play L", ""}},
		WhiteMomentary7mm{{to_mm<72>(122.85), to_mm<72>(41.6), Center, "Bank L", ""}},
		WhiteMomentary7mm{{to_mm<72>(164.37), to_mm<72>(41.88), Center, "Bank R", ""}},
		WhiteMomentary7mm{{to_mm<72>(205.55), to_mm<72>(49.77), Center, "Play R", ""}},
		Davies1900hBlackKnob{{to_mm<72>(249.91), to_mm<72>(54.27), Center, "Pitch R", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(95.87), to_mm<72>(90.17), Center, "Sample L", ""}, 0.5f},
		WhiteMomentary7mm{{to_mm<72>(143.63), to_mm<72>(69.21), Center, "Edit Sample", ""}},
		Davies1900hBlackKnob{{to_mm<72>(191.25), to_mm<72>(90.02), Center, "Sample R", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(37.24), to_mm<72>(123.0), Center, "Start Pos. L", ""}, 0.5f},
		WhiteMomentary7mm{{to_mm<72>(127.63), to_mm<72>(139.65), Center, "REC", ""}},
		WhiteMomentary7mm{{to_mm<72>(159.13), to_mm<72>(139.65), Center, "Bank", ""}},
		Davies1900hBlackKnob{{to_mm<72>(249.6), to_mm<72>(122.95), Center, "Start Pos. R", ""}, 0.5f},
		WhiteMomentary7mm{{to_mm<72>(21.08), to_mm<72>(179.74), Center, "Reverse L", ""}},
		Davies1900hBlackKnob{{to_mm<72>(69.36), to_mm<72>(189.23), Center, "Length L", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(143.45), to_mm<72>(183.69), Center, "Sample", ""}, 0.5f},
		Davies1900hBlackKnob{{to_mm<72>(218.16), to_mm<72>(189.23), Center, "Length R", ""}, 0.5f},
		WhiteMomentary7mm{{to_mm<72>(266.32), to_mm<72>(179.74), Center, "Reverse R", ""}},
		AnalogJackOutput4ms{{to_mm<72>(27.2), to_mm<72>(248.15), Center, "Out L", ""}},
		AnalogJackInput4ms{{to_mm<72>(66.74), to_mm<72>(248.15), Center, "Gate Out", ""}},
		AnalogJackInput4ms{{to_mm<72>(106.47), to_mm<72>(244.71), Center, "Left Rec", ""}},
		GateJackInput4ms{{to_mm<72>(143.45), to_mm<72>(263.71), Center, "Rec Jack", ""}},
		AnalogJackInput4ms{{to_mm<72>(180.84), to_mm<72>(244.71), Center, "Right Rec", ""}},
		AnalogJackInput4ms{{to_mm<72>(220.3), to_mm<72>(248.15), Center, "1V/Oct R", ""}},
		AnalogJackOutput4ms{{to_mm<72>(260.66), to_mm<72>(248.15), Center, "Out R", ""}},
		GateJackInput4ms{{to_mm<72>(27.2), to_mm<72>(283.45), Center, "Play Jack L", ""}},
		AnalogJackOutput4ms{{to_mm<72>(68.86), to_mm<72>(283.45), Center, "End Out L", ""}},
		GateJackInput4ms{{to_mm<72>(110.74), to_mm<72>(283.45), Center, "Reverse Jack L", ""}},
		GateJackInput4ms{{to_mm<72>(177.15), to_mm<72>(283.45), Center, "Reverse Jack R", ""}},
		AnalogJackOutput4ms{{to_mm<72>(219.17), to_mm<72>(283.45), Center, "End Out R", ""}},
		GateJackInput4ms{{to_mm<72>(260.66), to_mm<72>(283.45), Center, "Play Jack R", ""}},
		AnalogJackInput4ms{{to_mm<72>(27.2), to_mm<72>(319.51), Center, "Length CV L", ""}},
		AnalogJackInput4ms{{to_mm<72>(73.91), to_mm<72>(319.37), Center, "Start Pos CV L", ""}},
		AnalogJackInput4ms{{to_mm<72>(120.67), to_mm<72>(319.51), Center, "Sample CV L", ""}},
		AnalogJackInput4ms{{to_mm<72>(167.23), to_mm<72>(319.45), Center, "Sample CV R", ""}},
		AnalogJackInput4ms{{to_mm<72>(213.99), to_mm<72>(319.45), Center, "Start Pos CV R", ""}},
		AnalogJackInput4ms{{to_mm<72>(260.66), to_mm<72>(319.45), Center, "Length CV R", ""}},
		RedGreenBlueLight{{to_mm<72>(14.74), to_mm<72>(227.18), Center, "LED L", ""}},
		RedGreenBlueLight{{to_mm<72>(133.57), to_mm<72>(235.07), Center, "Monitor LED", ""}},
		RedGreenBlueLight{{to_mm<72>(153.69), to_mm<72>(235.07), Center, "Busy LED", ""}},
		RedGreenBlueLight{{to_mm<72>(273.54), to_mm<72>(227.18), Center, "LED R", ""}},
}};

    enum class Elem {
        PitchLKnob,
        PlayLButton,
        BankLButton,
        BankRButton,
        PlayRButton,
        PitchRKnob,
        SampleLKnob,
        EditSampleButton,
        SampleRKnob,
        StartPos_LKnob,
        RecButton,
        BankButton,
        StartPos_RKnob,
        ReverseLButton,
        LengthLKnob,
        SampleKnob,
        LengthRKnob,
        ReverseRButton,
        OutLOut,
        GateOutIn,
        LeftRecIn,
        RecJackIn,
        RightRecIn,
        _1V_OctRIn,
        OutROut,
        PlayJackLIn,
        EndOutLOut,
        ReverseJackLIn,
        ReverseJackRIn,
        EndOutROut,
        PlayJackRIn,
        LengthCvLIn,
        StartPosCvLIn,
        SampleCvLIn,
        SampleCvRIn,
        StartPosCvRIn,
        LengthCvRIn,
        LedLLight,
        MonitorLedLight,
        BusyLedLight,
        LedRLight,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobPitch_L, 
        KnobPitch_R, 
        KnobSample_L, 
        KnobSample_R, 
        KnobStart_Pos__L, 
        KnobStart_Pos__R, 
        KnobLength_L, 
        KnobSample, 
        KnobLength_R, 
        NumKnobs,
    };
    
    enum {
        SwitchPlay_L, 
        SwitchBank_L, 
        SwitchBank_R, 
        SwitchPlay_R, 
        SwitchEdit_Sample, 
        SwitchRec, 
        SwitchBank, 
        SwitchReverse_L, 
        SwitchReverse_R, 
        NumSwitches,
    };
    
    enum {
        InputGate_Out, 
        InputLeft_Rec, 
        InputRec_Jack, 
        InputRight_Rec, 
        Input_1V_Oct_R, 
        InputPlay_Jack_L, 
        InputReverse_Jack_L, 
        InputReverse_Jack_R, 
        InputPlay_Jack_R, 
        InputLength_Cv_L, 
        InputStart_Pos_Cv_L, 
        InputSample_Cv_L, 
        InputSample_Cv_R, 
        InputStart_Pos_Cv_R, 
        InputLength_Cv_R, 
        NumInJacks,
    };
    
    enum {
        OutputOut_L, 
        OutputOut_R, 
        OutputEnd_Out_L, 
        OutputEnd_Out_R, 
        NumOutJacks,
    };
    
    enum {
        LedLed_L, 
        LedMonitor_Led, 
        LedBusy_Led, 
        LedLed_R, 
        NumDiscreteLeds,
    };
    
};
} // namespace MetaModule
