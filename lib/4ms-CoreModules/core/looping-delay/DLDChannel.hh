#include "CoreModules/elements/base_element.hh"
#include "src/flags.hh"
#include "src/controls.hh"
#include "src/params.hh"
#include "src/looping_delay.hh"

// #define DEBUG_LOGGING

#ifdef DEBUG_LOGGING
#include <cstdio>
#endif

namespace MetaModule
{

template <class Parent, class Mapping>
class DLDChannel
{
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
	DLDChannel(Parent* parent_)
		: parent(parent_)
		, params(controls, flags)
		, looping_delay(params, flags)
		, audioBufferFillCount(0)
		, uiUpdateCounter(0)
		, sampleRate(looping_delay.DefaultSampleRate)
		{
			for (auto& sample : outBlock)
			{
				for (auto& channel : sample.chan)
				{
					channel = 0;
				}
			}
		}

    void update()
    {
		params.timer.inc();

		if (uiUpdateCounter >= params.currentSampleRate/uiUpdateRateHz)
		{
			uiUpdateCounter = 0;

			sideloadDrivers();
			handleAltParameters();
		}

		if (audioBufferFillCount >= inBlock.size())
		{
			audioBufferFillCount = 0;

			params.update();
			looping_delay.update(inBlock, outBlock);		
		}

		packUnpackBlockBuffers();
		audioBufferFillCount++;
		uiUpdateCounter++;
    }

    void set_samplerate(float sr) {

		auto newSampleRate = uint32_t(std::round(sr));

		if (newSampleRate != sampleRate)
		{
			#ifdef DEBUG_LOGGING
			printf("Switch sample rate %u -> %u\n", sampleRate, newSampleRate);
			#endif

			sampleRate = newSampleRate;

			looping_delay.set_samplerate(sampleRate);
		}
    }

	void set_ping_time(uint32_t ping_time) {
		looping_delay.set_ping_time(ping_time);
	}

	uint32_t get_ping_time() {
		return looping_delay.get_ping_time_at_default_samplerate();
	}

public:
	struct ModuleIO_t
	{
		float audioIn;
		float audioOut;
		bool pingButtonIn;
		bool pingJackIn;
		bool pingOut;
		bool clockOut;
	};
	ModuleIO_t io;

private:
	void sideloadDrivers()
	{
		auto ConvertKnobFunc = [](float val) -> int16_t
		{
			// 0..1 -> 0..4095
			return std::clamp<int16_t>(val * 4096.f, 0, 4095);
		};

		auto ConvertCVFunc = [](float val) -> int16_t
		{
			// 0..+5V -> 0..4095
			return std::clamp<int16_t>(val / 5.f * 4096.f, 0, 4095);
		};

		auto ConvertTimeCVFunc = [](float val) -> int16_t
		{
			// -5V..+5V -> 4095..0
			return std::clamp<int16_t>((5.f - val)/10.f * 4096.f, 0, 4095);
		};

		auto Convert3WaySwitchStateFunc = [](auto inState) -> LDKit::Mocks::SwitchPos
		{
			switch (inState)
			{
				case Toggle3pos::State_t::DOWN: return LDKit::Mocks::SwitchPos::Down;
				case Toggle3pos::State_t::UP: return LDKit::Mocks::SwitchPos::Up;
				case Toggle3pos::State_t::CENTER: return LDKit::Mocks::SwitchPos::Center;
				default: return LDKit::Mocks::SwitchPos::Invalid;
			}
		};

		// Knobs
		controls.sideload_pot(LDKit::TimePot,      ConvertKnobFunc(getState<Mapping::TimeKnob>()));
		controls.sideload_pot(LDKit::FeedbackPot,  ConvertKnobFunc(getState<Mapping::FeedbackKnob>()));
		controls.sideload_pot(LDKit::DelayFeedPot, ConvertKnobFunc(getState<Mapping::DelayFeedKnob>()));
		controls.sideload_pot(LDKit::MixPot,       ConvertKnobFunc(getState<Mapping::MixKnob>()));

		// CVs
		controls.sideload_cv(LDKit::TimeCV,      ConvertTimeCVFunc(getInput<Mapping::TimeInput>().value_or(0)));
		controls.sideload_cv(LDKit::FeedbackCV,  ConvertCVFunc(getInput<Mapping::FeedbackInput>().value_or(0)));
		controls.sideload_cv(LDKit::DelayFeedCV, ConvertCVFunc(getInput<Mapping::DelayFeedInput>().value_or(0)));
		// controls.sideload_cv(LDKit::MixCV,       ConvertCVFunc(getInput<Mapping::MixInput>().value_or(0)));

		// LEDs
		setLED<Mapping::ReverseButton>(controls.rev_led.sideload_get());
		setLED<Mapping::HoldButton>(controls.inf_led.sideload_get());

		setLED<Mapping::TimeLight>(params.timer.loop_led.sideload_get() ? 1.0f: 0.0f);

		// Buttons
		controls.rev_button.register_state(getState<Mapping::ReverseButton>() == MomentaryButton::State_t::PRESSED);
		controls.inf_button.register_state(getState<Mapping::HoldButton>() == MomentaryButton::State_t::PRESSED);

		// Trigger Inputs
		controls.rev_jack.register_state(getInput<Mapping::ReverseInput>().value_or(0) > TriggerThresholdInVolt);
		controls.inf_jack.register_state(getInput<Mapping::HoldInput>().value_or(0) > TriggerThresholdInVolt);

		// Switch
		controls.time_switch.sideload_set(Convert3WaySwitchStateFunc(getState<Mapping::TimeModSwitch>()));

		// Clock outputs
		setOutput<Mapping::LoopOutput>(params.timer.loop_out.sideload_get() * TriggerOutputFullScaleInVolt);

		// Forward to/from module level
		io.pingOut = controls.ping_led.sideload_get();
		io.clockOut = params.timer.clk_out.sideload_get();
		params.timer.ping_jack.register_state(io.pingJackIn);
		controls.ping_button.register_state(io.pingButtonIn);
	}

	void handleAltParameters()
	{
		params.settings.soft_clip                = getState<Mapping::SoftClipEnabledAlt>() == 0;
		params.settings.auto_mute                = getState<Mapping::AutoMuteEnabledAlt>() == 0;
		params.settings.auto_unquantize_timejack = getState<Mapping::TimeUnquantizedAlt>() == 1;
		params.settings.stereo_mode              = getState<Mapping::StereoModeAlt>() == 1;
		params.settings.log_delay_feed           = getState<Mapping::DelayFeedTaperModeAlt>() == 0;
		params.settings.rev_jack                 = getState<Mapping::ReverseInputModeAlt>() == 0 ? LDKit::GateType::Trig : LDKit::GateType::Gate;
		params.settings.inf_jack                 = getState<Mapping::HoldInputModeAlt>()    == 0 ? LDKit::GateType::Trig : LDKit::GateType::Gate;

		static constexpr std::array<uint32_t, 7> CrossfadeSamples      = {1, 96, 192, 384, 1200, 4800, 12000};
		static constexpr std::array<uint32_t, 7> WriteCrossfadeSamples = {1, 96, 192, 192, 192,  4800,  4800};

		auto AdaptLengthToSampleRateFunc = [this](auto lengthInSamples)
		{
			// make sure this never becomes zero
			return std::max<uint32_t>(1, uint32_t(lengthInSamples * sampleRate / looping_delay.DefaultSampleRate));
		};

		params.settings.crossfade_samples       = AdaptLengthToSampleRateFunc(CrossfadeSamples[getState<Mapping::CrossFadeTimeAlt>()]);
		params.settings.crossfade_rate          = params.settings.calc_fade_increment(params.settings.crossfade_samples);
		params.settings.write_crossfade_samples = AdaptLengthToSampleRateFunc(WriteCrossfadeSamples[getState<Mapping::CrossFadeTimeAlt>()]);
		params.settings.write_crossfade_rate    = params.settings.calc_fade_increment(params.settings.write_crossfade_samples);

		static constexpr std::array<PingMethod,5> PingMethods = {
			PingMethod::LINEAR_AVERAGE_4,
			PingMethod::MOVING_AVERAGE_2,
			PingMethod::ONE_TO_ONE,
			PingMethod::IGNORE_FLAT_DEVIATION_10,
			PingMethod::IGNORE_PERCENT_DEVIATION
		};

		params.settings.ping_method = PingMethods[getState<Mapping::PingMethodAlt>()];
	}

private:
	void packUnpackBlockBuffers()
	{
		constexpr float AudioFullScale = ((1<<(AudioStreamConf::SampleBits-1)) -1);

		auto clamp = [](auto val, auto max_amplitude)
		{
			return std::min(std::max(val, -max_amplitude), max_amplitude);
		};

		auto InputConversionFunc = [clamp](auto inputInV) -> int32_t
		{
			float normalized = clamp(inputInV / AudioInputFullScaleInVolt, 1.0f);
			return int32_t(normalized * AudioFullScale);
		};

		auto OutputConversionFunc = [](auto output) -> float
		{
			return float(output) / AudioFullScale * AudioOutputFullScaleInVolt;
		};
		
		inBlock[audioBufferFillCount] = {InputConversionFunc(getInput<Mapping::ReturnInput>().value_or(0)), InputConversionFunc(io.audioIn)};

		io.audioOut = OutputConversionFunc(outBlock[audioBufferFillCount].chan[1]);
		setOutput<Mapping::SendOutput>(OutputConversionFunc(outBlock[audioBufferFillCount].chan[0]));
	}

private:
	static constexpr float AudioInputFullScaleInVolt    = 10.0f;
	static constexpr float AudioOutputFullScaleInVolt   = 10.0f;
	static constexpr float TriggerThresholdInVolt       = 0.1f;
	static constexpr float TriggerOutputFullScaleInVolt = 5.0f;
	static constexpr std::size_t uiUpdateRateHz = 3000;

private:
	LDKit::Controls controls;
	LDKit::Flags flags;
	LDKit::Params params;
	LDKit::LoopingDelay looping_delay;

private:
	AudioStreamConf::AudioInBlock inBlock;
	AudioStreamConf::AudioOutBlock outBlock;
	std::size_t audioBufferFillCount;
	std::size_t uiUpdateCounter;

private:
	uint32_t sampleRate;
}; 

}
