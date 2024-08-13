#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "helpers/envelope_follower.hh"
#include "info/PI_info.hh"

#include "l4/DCBlock.h"
#include "l4/PeakDetector.h"

namespace MetaModule
{

class PICore : public SmartCoreProcessor<PIInfo> {
	using Info = PIInfo;
	using ThisCore = PICore;
	using enum Info::Elem;

private:
	enum GainRange_t { LOW = 0, MEDIUM = 1, HIGH = 2 };
	enum GateState_t { IDLE, TRIGGERED };
	enum Mode_t { FOLLOW, GEN };

public:
	PICore()
		: ticks(0)
		, sampleRate(48000.f)
		, mode(FOLLOW)
		, gateState(IDLE)
		, dcBlocker(DCBlockerFactor) {
			envelope.setAttack(0.005f);
		}

	void update() override {
		ticks++;

		auto scaledInput = 0.f;

		if (auto input = getInput<In>(); input) {
			auto filteredInput = dcBlocker(*input);
			auto maximumGain = readMaximumGain();
			scaledInput = std::clamp(filteredInput * (getState<SensitivityKnob>() * maximumGain), -12.f, 12.f);

			checkGateTrigger(scaledInput);
		}

		updateGate(ticks);

		setDecayTime();
		readEnvelopeMode();

		auto envelopePOut = 0.f;

		envelopePOut = generateEnvelope(mode == FOLLOW ? scaledInput : gateState == TRIGGERED ? gateOutHighVoltage : gateOutLowVoltage);

		auto envelopeNOut = envelopeHighVoltage - envelopePOut;

		setLED<GateLight>(gateState == TRIGGERED ? 1.f : 0.f);
		setOutput<GateOut>(gateState == TRIGGERED ? gateOutHighVoltage : gateOutLowVoltage);

		setOutput<Env_OutPOut>(envelopePOut);
		setLED<EnvPLight>(envelopePOut / envelopeHighVoltage);
		setOutput<Env_OutNOut>(envelopeNOut);
		setLED<EnvNLight>(envelopeNOut/ envelopeHighVoltage);

		setOutput<Env_Out>(envelopePOut * getState<Env_LevelKnob>());
		setOutput<Inv_Out>(envelopeNOut * getState<Inv_LevelKnob>());

		setOutput<AudioOut>(scaledInput);
		auto sensLight = senseEnvelope(scaledInput);
		setLED<Sens_Light>(std::array<float,3>{(sensLight - 6.f) / 3.f,0.f,sensLight / 3.5f});
	}

	float readMaximumGain() {
		auto gainMode = getState<GainSwitch>();

		if (gainMode == Toggle3posHoriz::State_t::LEFT) {
			return maximumGains[LOW];
		} else if (gainMode == Toggle3posHoriz::State_t::CENTER) {
			return maximumGains[MEDIUM];
		} else {
			return maximumGains[HIGH];
		}
	}

	void checkGateTrigger(float input) {
		if (gateState == IDLE) {
			if (input >= gateThresholdInV) {
				gateState = TRIGGERED;
				lastGateTriggerInTicks = ticks;
			}
		}
	}

	void updateGate(uint32_t now)
	{
		if (gateState == TRIGGERED) {
			auto gateLengthInTicks = getState<SustainKnob>() * (maximumGateLengthInTicks - minimumGateLengthInTicks) +
									 minimumGateLengthInTicks;

			if (now > lastGateTriggerInTicks + gateLengthInTicks) {
				gateState = IDLE;
			}
		}
	}

	void readEnvelopeMode()
	{
		auto envMode = getState<EnvModeSwitch>();

		if(envMode == Toggle2posHoriz::State_t::LEFT) {
			mode = FOLLOW;
		} else {
			mode = GEN;
		}
	}

	void setDecayTime()
	{
		envelope.setDecay(getState<Env_DecayKnob>() * (maximumDecayTimeInS - minimumDecayTimeInS) + minimumDecayTimeInS);
	}

	float generateEnvelope(float input)
	{
		static constexpr float envelopeInputGain = 3.2f;
		static constexpr float envelopeInputOffset = -0.29f;

		input *= envelopeInputGain;
		input += envelopeInputOffset;

		auto envelopeOutput = envelope(std::clamp(input, 0.f, envelopeHighVoltage));

		return envelopeOutput;
	}

	void set_samplerate(float sr) override {
		envelope.setSamplerate(sr);
		senseEnvelope.setSamplerate(sr);
		sampleRate = sr;
		minimumGateLengthInTicks = minimumGateLengthInS * sampleRate;
		maximumGateLengthInTicks = maximumGateLengthInS * sampleRate;
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	static constexpr float minimumGateLengthInS = 0.005f;
	static constexpr float maximumGateLengthInS = 0.5f;

	static constexpr float gateOutLowVoltage = 0.f;
	static constexpr float gateOutHighVoltage = 8.f;

	static constexpr float gateThresholdInV = 3.5f;

	static constexpr float envelopeHighVoltage = 9.f;

	static constexpr float minimumDecayTimeInS = 0.022f;
	static constexpr float maximumDecayTimeInS = 2.2f;

	static constexpr std::array<float, 3> maximumGains{2.f, 20.f, 500.f};

private:
	uint32_t ticks;
	float sampleRate;
	Mode_t mode;
	GateState_t gateState;

	uint32_t minimumGateLengthInTicks;
	uint32_t maximumGateLengthInTicks;
	uint32_t lastGateTriggerInTicks;

private:
	static constexpr float DCBlockerFactor = 0.9995f;
	DCBlock dcBlocker;

	EnvelopeFollower envelope;
	PeakDetector senseEnvelope;
};

} // namespace MetaModule
