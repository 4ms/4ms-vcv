#pragma once
#include "CoreModules/4ms/4ms_elements.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/elements/element_info.hh"
#include <array>

namespace MetaModule
{
struct FollowInfo : ModuleInfoBase {
    static constexpr std::string_view slug{"Follow"};
    static constexpr std::string_view description{"Follower"};
    static constexpr uint32_t width_hp = 4;
    static constexpr std::string_view svg_filename{"res/modules/Follow_artwork.svg"};
    static constexpr std::string_view png_filename{"4ms/fp/Follow.png"};

    using enum Coords;

    static constexpr std::array<Element, 6> Elements{{
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(46.77), Center, "Rise", ""}, 0.0f},
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(94.96), Center, "Fall", ""}, 0.0f},
		Knob9mm{{to_mm<72>(28.8), to_mm<72>(143.15), Center, "Thresh", ""}, 0.0f},
		AnalogJackInput4ms{{to_mm<72>(28.8), to_mm<72>(216.85), Center, "Input", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(265.04), Center, "Gate", ""}},
		AnalogJackOutput4ms{{to_mm<72>(28.8), to_mm<72>(313.23), Center, "Env", ""}},
}};

    enum class Elem {
        RiseKnob,
        FallKnob,
        ThreshKnob,
        InputIn,
        GateOut,
        EnvOut,
    };

    // Legacy naming (safe to remove once all legacy 4ms CoreModules are converted)
    
    enum {
        KnobRise, 
        KnobFall, 
        KnobThresh, 
        NumKnobs,
    };
    
    
    enum {
        InputInput, 
        NumInJacks,
    };
    
    enum {
        OutputGate, 
        OutputEnv, 
        NumOutJacks,
    };
    
    
};
} // namespace MetaModule
