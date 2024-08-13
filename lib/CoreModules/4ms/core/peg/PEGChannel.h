#pragma once

#include "peg-common/peg_base.hh"
#include "peg/envelope_calcs.hh"

#include "helpers/FlipFlop.h"
#include "helpers/EdgeDetector.h"
#include <algorithm>

using namespace MetaModule::PEG;

namespace MetaModule
{

template<class Parent, class Mapping>
class PEGChannel 
{
	friend Parent;

private:
    template<typename Parent::Info::Elem EL>
	void setOutput(auto val)
	{
		return parent->template setOutput<EL>(val);
	}

	template<typename Parent::Info::Elem EL>
	auto getInput()
	{
		return parent->template getInput<EL>();
	}

	template<typename Parent::Info::Elem EL, typename VAL>
	void setLED(const VAL &value)
	{
		return parent->template setLED<EL>(value);
	}

	template<typename Parent::Info::Elem EL>
	auto getState()
	{
		return parent->template getState<EL>();
	}

private:
    Parent* parent;

public:
    PEGChannel(Parent* parent_)
        : parent(parent_), pingIn(1.f, 2.f)
        , cycleIn(1.f, 2.f)
        , triggerIn(1.f, 2.f)
        , qntIn(1.f, 2.f)
        , asyncIn(1.f, 2.f)
    {
        // TODO: maybe calling these is not required
        sideloadDrivers();
        sideloadSystemSettings();
        peg.update_all_envelopes();

        //PEG always starts in Sync mode
        peg.set_sync_mode(true); 

        peg.settings.shift_value = 0.5f * 4095.f;

        peg.settings.cycle_jack_behavior = CYCLE_JACK_BOTH_EDGES_TOGGLES;
    };

    void update() {
        sideloadDrivers();
        sideloadSystemSettings();
        peg.update();
    }

    void doDACUpdate()
    {
        peg.update_all_envelopes();
    }

    void toggleInput(auto input)
    {
        peg.digio.CycleJack.sideload_set(cycleIn(input.value_or(0.f)));
    }

    enum MainMode {EOF_GATE, EOF_TRIG, TAP_GATE, TAP_TRIG};
    enum SecondaryMode {EOR_GATE, EOR_TRIG, HR_GATE, HR_TRIG};

    void setMainMode(MainMode mode)
    {
        if (mode == EOF_GATE or mode == TAP_GATE)
        {
            peg.settings.trigout_is_trig = 0;
        }
        else
        {
            peg.settings.trigout_is_trig = 1;
        }

        if (mode == EOF_GATE or mode == EOF_TRIG)
        {
            peg.settings.trigout_function = TRIGOUT_IS_ENDOFFALL;
        }
        else
        {
            peg.settings.trigout_function = TRIGOUT_IS_TAPCLKOUT;
        }
    }

    void setSecondaryMode(SecondaryMode mode)
    {
        if (mode == EOR_GATE or mode == HR_GATE)
        {
            peg.settings.trigout_secondary_is_trig = 0;
        }
        else
        {
            peg.settings.trigout_secondary_is_trig = 1;
        }

        if (mode == EOR_GATE or mode == EOR_TRIG)
        {
            peg.settings.trigout_secondary_function = TRIGOUT_IS_ENDOFRISE;
        }
        else
        {
            peg.settings.trigout_secondary_function = TRIGOUT_IS_HALFRISE;
        }
    }

private:
    void sideloadDrivers() {
        auto MapKnobFunc = [](auto val) -> uint16_t {
            return uint16_t(val * 4095.f);
        };

        peg.adc_dma_buffer[POT_SCALE] = MapKnobFunc(getState<Mapping::ScaleKnob>());
        peg.adc_dma_buffer[POT_OFFSET] = MapKnobFunc(getState<Mapping::BiNPolarButton>() == LatchingButton::State_t::DOWN ? 0.5f : 1.f);
        

        auto MapKnobCvFunc = [](auto knobval, auto cvval) -> uint16_t {
			return std::clamp(knobval + cvval / CVInputFullScaleInV, 0.f, 1.f) * 4095;
        };
		// Combine CV and knob voltage to simulate analog circuitry on PEG hardware
        peg.adc_dma_buffer[POT_SHAPE] = MapKnobCvFunc(getState<Mapping::CurveKnob>(), getInput<Mapping::CurveJackIn>().value_or(0.f));
        peg.adc_dma_buffer[POT_SKEW] = MapKnobCvFunc(getState<Mapping::SkewKnob>(), getInput<Mapping::SkewJackIn>().value_or(0.f));
        peg.adc_dma_buffer[POT_DIVMULT] = MapKnobCvFunc(getState<Mapping::PingDivMultKnob>(), getInput<Mapping::DivJackIn>().value_or(0.f));
        peg.adc_dma_buffer[CV_SHAPE] = CVZeroVoltsAdcValue; 
        peg.adc_dma_buffer[CV_DIVMULT] = CVZeroVoltsAdcValue;

        peg.digio.PingBut.sideload_set(getState<Mapping::PingButton>() == MomentaryButton::State_t::PRESSED);
        peg.digio.CycleBut.sideload_set(getState<Mapping::CycleButton>() == MomentaryButton::State_t::PRESSED);
        
        auto qntJackIn = getInput<Mapping::QntIn>().value_or(0.f);
        auto asyncJackIn = getInput<Mapping::AsyncIn>().value_or(0.f);

        if (qntEdge(qntIn(qntJackIn))) 
        {
            peg.settings.trigin_function = TRIGIN_IS_QNT;
        }
        else if(not asyncEdgeEnabled and asyncIn(asyncJackIn))
        {
            peg.settings.trigin_function = TRIGIN_IS_ASYNC_SUSTAIN;
        }
        else if (asyncEdgeEnabled and asyncEdge(asyncIn(asyncJackIn)))
        {
            peg.settings.trigin_function = TRIGIN_IS_ASYNC;
        }

        if (peg.settings.trigin_function == TRIGIN_IS_QNT)
        {
            peg.digio.TrigJack.sideload_set(qntIn(qntJackIn));
        }
        else
        {
            peg.digio.TrigJack.sideload_set(asyncIn(asyncJackIn));
        }

        // TODO: ping input originally has internal lowpass filtering
        if (pingEdge(pingIn(getInput<Mapping::PingJackIn>().value_or(0.f)))) {
            peg.pingEdgeIn();
        }

        setOutput<Mapping::EoFOut>(peg.digio.EOJack.sideload_get() ? TriggerOutputInV : 0.f);

        // Secondary out with directly coupled LED
        // TODO: convert LED to unicolor
        setOutput<Mapping::SecondaryOut>(peg.digio.EOJackSecondary.sideload_get() ? TriggerOutputInV : 0.f);
        setLED<Mapping::SecondaryLight>(peg.digio.EOJackSecondary.sideload_get());

        auto MapDACFunc = [](auto val) -> float {
            return float(val) / 4095.f;
        };

        setOutput<Mapping::EnvOut>(MapDACFunc(peg.dac_vals[0]) * EnvelopeOutFullScaleInV + EnvelopeOutOffsetInV);
        setOutput<Mapping::_5VEnvOut>(MapDACFunc(peg.dac_vals[1]) * 5.f);

        auto PWMToFloatFunc = [](uint16_t pwm_val) -> float {
            return float(pwm_val) / float(4095);
        };

        setLED<Mapping::EnvOutLight>(MapDACFunc(peg.dac_vals[1]));
        setLED<Mapping::CycleButton>(std::array<float, 3>{PWMToFloatFunc(peg.pwm_vals[PWM_CYCLEBUT_R]),
                                                    PWMToFloatFunc(peg.pwm_vals[PWM_CYCLEBUT_G]),
                                                    PWMToFloatFunc(peg.pwm_vals[PWM_CYCLEBUT_B])});
        setLED<Mapping::PingButton>(std::array<float, 3>{PWMToFloatFunc(peg.pwm_vals[PWM_PINGBUT_R]),
                                                PWMToFloatFunc(peg.pwm_vals[PWM_PINGBUT_G]),
                                                PWMToFloatFunc(peg.pwm_vals[PWM_PINGBUT_B])});
        setLED<Mapping::EofLight>(PWMToFloatFunc(peg.pwm_vals[PWM_EOF_LED]));

        setLED<Mapping::BiNPolarButton>(getState<Mapping::BiNPolarButton>() == LatchingButton::State_t::DOWN);
    }

    void sideloadSystemSettings() {
        peg.settings.limit_skew = getState<Mapping::SkewLimitAltParam>() == 1 ? 1 : 0;
        peg.settings.free_running_ping = getState<Mapping::FreeNRunningPingAltParam>() == 0 ? 1 : 0;

        /*
        ON: AR mode (Attack-Release). A gate on the Async jack will be ignored, and the envelope will attack and
        immediate release (no sustain/hold). The is identical to converting a gate to a trigger before running it into the
        Async jack
        */
        asyncEdgeEnabled = getState<Mapping::AsyncModeAltParam>() == 1;
    }

private:
    static constexpr float CVInputFullScaleInV = 10.0f;
    static constexpr uint16_t CVZeroVoltsAdcValue = 2048;
    static constexpr float TriggerOutputInV = 5.0f;
    static constexpr float EnvelopeOutFullScaleInV = -20.0f;
    static constexpr float EnvelopeOutOffsetInV = 10.0f;

private:
	PEG::PEGEnvelopeCalcs env_calcs;
    PEG::PEGBase peg{&env_calcs};

private:
    FlipFlop pingIn;
    FlipFlop cycleIn;
    FlipFlop triggerIn;
    EdgeDetector pingEdge;

    FlipFlop qntIn;
    FlipFlop asyncIn;
    EdgeDetector qntEdge;
    EdgeDetector asyncEdge;

    bool asyncEdgeEnabled;
};


}
