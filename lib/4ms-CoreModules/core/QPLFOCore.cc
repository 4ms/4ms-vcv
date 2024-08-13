#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/QPLFO_info.hh"

#include "qplfo/qplfo.hh"
#include "helpers/EdgeDetector.h"

#include <algorithm>

namespace MetaModule
{

class QPLFOCore : public SmartCoreProcessor<QPLFOInfo> {
	using Info = QPLFOInfo;
	using ThisCore = QPLFOCore;
	using enum Info::Elem;

public:
	QPLFOCore() : tickCounter(0)
	{
		set_samplerate(48000.0f);
	}

	void update() override
	{
		tickCounter += ticksPerSample;
		while (tickCounter >= 1.0f)
		{
			mod.doTick();
			tickCounter -= 1.0f;
		}

		sideloadDrivers();

		for (std::size_t i=0; i<4; i++)
		{
			mod.updateNextChannel();
		}
	}

	void sideloadDrivers()
	{
		channelOn[0] = getState<On1Button>() == LatchingButton::State_t::DOWN;
		channelOn[1] = getState<On2Button>() == LatchingButton::State_t::DOWN;
		channelOn[2] = getState<On3Button>() == LatchingButton::State_t::DOWN;
		channelOn[3] = getState<On4Button>() == LatchingButton::State_t::DOWN;

		mod.tapButtons[0] = getState<Ping1Button>() == MomentaryButton::State_t::PRESSED;
		mod.tapButtons[1] = getState<Ping2Button>() == MomentaryButton::State_t::PRESSED;
		mod.tapButtons[2] = getState<Ping3Button>() == MomentaryButton::State_t::PRESSED;
		mod.tapButtons[3] = getState<Ping4Button>() == MomentaryButton::State_t::PRESSED;

		mod.pingInputs[0] = getInput<Ping1JackIn>().value_or(0);
		mod.pingInputs[1] = getInput<Ping2JackIn>().value_or(0);
		mod.pingInputs[2] = getInput<Ping3JackIn>().value_or(0);
		mod.pingInputs[3] = getInput<Ping4JackIn>().value_or(0);

		mod.resetInputs[0] = getInput<Reset1In>().value_or(0);
		mod.resetInputs[1] = getInput<Reset2In>().value_or(0);
		mod.resetInputs[2] = getInput<Reset3In>().value_or(0);
		mod.resetInputs[3] = getInput<Reset4In>().value_or(0);

		if (onEdgeDetectors[0](channelOn[0]) and getState<FireOnUnmuteCh1AltParam>() == 1) mod.resetInputs[0] = true;
		if (onEdgeDetectors[1](channelOn[1]) and getState<FireOnUnmuteCh2AltParam>() == 1) mod.resetInputs[1] = true;
		if (onEdgeDetectors[2](channelOn[2]) and getState<FireOnUnmuteCh3AltParam>() == 1) mod.resetInputs[2] = true;
		if (onEdgeDetectors[3](channelOn[3]) and getState<FireOnUnmuteCh4AltParam>() == 1) mod.resetInputs[3] = true;

		setLED<Led1Light>(mod.outputs[0]);
		setLED<Led2Light>(mod.outputs[1]);
		setLED<Led3Light>(mod.outputs[2]);
		setLED<Led4Light>(mod.outputs[3]);

		setLED<On1Button>(channelOn[0]);
		setLED<On2Button>(channelOn[1]);
		setLED<On3Button>(channelOn[2]);
		setLED<On4Button>(channelOn[3]);

		setLED<Ping1Button>(mod.tapLEDs[0]);
		setLED<Ping2Button>(mod.tapLEDs[1]);
		setLED<Ping3Button>(mod.tapLEDs[2]);
		setLED<Ping4Button>(mod.tapLEDs[3]);

		auto OutputFunc = [this](auto val)
		{
			auto result = val * OutputFullScaleInV;

			if (getState<Info::Elem::OutputRangeAltParam>() == 1)
			{
				result -= OutputFullScaleInV / 2.0f;
			}

			return result;
		};

		setOutput<Out1Out>(channelOn[0] ? OutputFunc(mod.outputs[0]) : 0.0f);
		setOutput<Out2Out>(channelOn[1] ? OutputFunc(mod.outputs[1]) : 0.0f);
		setOutput<Out3Out>(channelOn[2] ? OutputFunc(mod.outputs[2]) : 0.0f);
		setOutput<Out4Out>(channelOn[3] ? OutputFunc(mod.outputs[3]) : 0.0f);

		mod.setADCChannel(0, std::clamp(getState<Skew1Knob>() + getInput<Skew1CvIn>().value_or(0) / CVInputFullScaleInV, 0.0f, 1.0f));
		mod.setADCChannel(1, std::clamp(getState<Skew2Knob>() + getInput<Skew2JackIn>().value_or(0) / CVInputFullScaleInV, 0.0f, 1.0f));
		mod.setADCChannel(2, std::clamp(getState<Skew3Knob>() + getInput<Skew3JackIn>().value_or(0) / CVInputFullScaleInV, 0.0f, 1.0f));
		mod.setADCChannel(3, std::clamp(getState<Skew4Knob>() + getInput<Skew4JackIn>().value_or(0) / CVInputFullScaleInV, 0.0f, 1.0f));

		
	}

	void set_samplerate(float sr) override {
		ticksPerSample = SystickFrequencyInHz / sr;
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	static constexpr float SystickFrequencyInHz = 10e3f;
	static constexpr float OutputFullScaleInV = 10.0f;
	static constexpr float CVInputFullScaleInV = 10.0f;

	float ticksPerSample;
	float tickCounter;

	std::array<bool,4> channelOn;
	std::array<EdgeDetector,4> onEdgeDetectors;

	QPLFO::Module mod;
};

} // namespace MetaModule
