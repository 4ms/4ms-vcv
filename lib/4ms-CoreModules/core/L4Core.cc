#include "info/L4_info.hh"
#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "l4/Tables.h"
#include "l4/DCBlock.h"
#include "l4/PeakDetector.h"

#include "processors/tools/expDecay.h"


namespace MetaModule
{

class L4Core : public SmartCoreProcessor<L4Info> {
	using Info = L4Info;
	using ThisCore = L4Core;
	using enum Info::Elem;

public:
	L4Core() :
		channel1DCBlocker(DCBlockerFactor), channel2DCBlocker(DCBlockerFactor), 
		channel3LeftDCBlocker(DCBlockerFactor), channel3RightDCBlocker(DCBlockerFactor),
		channel4LeftDCBlocker(DCBlockerFactor), channel4RightDCBlocker(DCBlockerFactor) {
	};

	void update() override {
		float outputLeft = 0.f;
		float outputRight = 0.f;

		auto channelLeft = 0.f;
		auto channelRight = channelLeft;

		if(auto input = getInput<In1In>(); input) {
			auto filteredInput = channel1DCBlocker(*input);
			channelLeft = filteredInput * PanningTable.lookup(1.f - getState<Pan1Knob>()) * LevelTable.lookup(getState<Level1Knob>());
			channelRight = filteredInput * PanningTable.lookup(getState<Pan1Knob>()) * LevelTable.lookup(getState<Level1Knob>());

			outputLeft += channelLeft;
			outputRight += channelRight;
		}

		setLED<Level1LedLight>(std::array<float,3>{0.f, channel1EnvelopeRight(gcem::abs(channelRight)) / LEDScaling , channel1EnvelopeLeft(gcem::abs(channelLeft)) / LEDScaling});

		channelLeft = 0.f;
		channelRight = channelLeft;

		if(auto input = getInput<In2In>(); input) {
			auto filteredInput = channel2DCBlocker(*input);
			channelLeft = filteredInput * PanningTable.lookup(1.f - getState<Pan2Knob>()) * LevelTable.lookup(getState<Level2Knob>());
			channelRight = filteredInput * PanningTable.lookup(getState<Pan2Knob>()) * LevelTable.lookup(getState<Level2Knob>());

			outputLeft += channelLeft;
			outputRight += channelRight;
		}

		setLED<Level2LedLight>(std::array<float,3>{0.f, channel2EnvelopeRight(gcem::abs(channelRight)) / LEDScaling , channel2EnvelopeLeft(gcem::abs(channelLeft)) / LEDScaling});

		channelLeft = 0.f;
		channelRight = channelLeft;

		if(auto inputL = getInput<In3LIn>(); inputL) {
			auto filteredInput = channel3LeftDCBlocker(*inputL);
			channelLeft = filteredInput * LevelTable.lookup(getState<Level3Knob>());
			channelRight =  channelLeft;
		}

		if(auto inputR = getInput<In3RIn>(); inputR) {
			auto filteredInput = channel3RightDCBlocker(*inputR);
			channelRight = filteredInput * LevelTable.lookup(getState<Level3Knob>());
		}

		outputLeft += channelLeft;
		outputRight += channelRight;

		setLED<Level3LedLight>(std::array<float,3>{0.f, channel3EnvelopeRight(gcem::abs(channelRight)) / LEDScaling , channel3EnvelopeLeft(gcem::abs(channelLeft)) / LEDScaling});

		channelLeft = 0.f;
		channelRight = channelLeft;

		if(auto inputL = getInput<In4LIn>(); inputL) {
			auto filteredInput = channel4LeftDCBlocker(*inputL);
			channelLeft = filteredInput * LevelTable.lookup(getState<Level4Knob>());
			channelRight = channelLeft;
		}

		if(auto inputR = getInput<In4RIn>(); inputR) {
			auto filteredInput = channel4RightDCBlocker(*inputR);
			channelRight = filteredInput * LevelTable.lookup(getState<Level4Knob>());
		}

		outputLeft += channelLeft;
		outputRight += channelRight;

		setLED<Level4LedLight>(std::array<float,3>{0.f, channel4EnvelopeRight(gcem::abs(channelRight)) / LEDScaling , channel4EnvelopeLeft(gcem::abs(channelLeft)) / LEDScaling});

		//+6dB output boost
		outputLeft *= 2.f;
		outputRight *= 2.f;

		auto headphoneOut = (outputLeft + outputRight) * LevelTable.lookup(getState<HeadphoneLevelKnob>());

		//-16.2dB attenuation in line mode
		if(getState<Mod__OR_LineSwitch>() == Toggle2posHoriz::State_t::RIGHT) {
			outputLeft *= 0.155f;
			outputRight *= 0.155f;
		}

		outputLeft *= LevelTable.lookup(getState<MainLevelKnob>());
		outputRight *= LevelTable.lookup(getState<MainLevelKnob>());

		auto outputLeftEnvelope = mainEnvelopeLeft(gcem::abs(outputLeft));
		auto outputRightEnvelope = mainEnvelopeRight(gcem::abs(outputRight));
		auto clippingLeft = outputLeftEnvelope >= 10.f ? 1.f : 0.f;
		auto clippingRight = outputRightEnvelope >= 10.f ? 1.f : 0.f;

		setLED<OutLLedLight>(std::array<float,3>{clippingLeft, 0.0f, outputLeftEnvelope / LEDScaling});
		setLED<OutRLedLight>(std::array<float,3>{clippingRight, outputRightEnvelope / LEDScaling, 0.f});

		setOutput<OutLeftOut>(std::clamp(outputLeft, -11.f, 11.f));
		setOutput<OutRightOut>(std::clamp(outputRight, -11.f, 11.f));
		setOutput<HeadphoneOut>(std::clamp(headphoneOut, -11.f, 11.f));
	}

	void set_samplerate(float sr) override {
		mainEnvelopeLeft.setSamplerate(sr);
		mainEnvelopeRight.setSamplerate(sr);
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	static constexpr float LEDScaling = 5.f;
	static constexpr float envelopeTimeConstant = 2000.f;
	static constexpr float DCBlockerFactor = 0.9995f;

	DCBlock channel1DCBlocker;
	DCBlock channel2DCBlocker;
	DCBlock channel3LeftDCBlocker;
	DCBlock channel3RightDCBlocker;
	DCBlock channel4LeftDCBlocker;
	DCBlock channel4RightDCBlocker;	

	PeakDetector channel1EnvelopeLeft;
	PeakDetector channel1EnvelopeRight;

	PeakDetector channel2EnvelopeLeft;
	PeakDetector channel2EnvelopeRight;

	PeakDetector channel3EnvelopeLeft;
	PeakDetector channel3EnvelopeRight;

	PeakDetector channel4EnvelopeLeft;
	PeakDetector channel4EnvelopeRight;

	PeakDetector mainEnvelopeLeft;
	PeakDetector mainEnvelopeRight;
};

} // namespace MetaModule
