#include "CoreModules/4ms/info/ENVVCA_info.hh"
#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"

#include "CoreModules/4ms/core/envvca/FollowInput.h"
#include "CoreModules/4ms/core/envvca/SSI2162.h"
#include "CoreModules/4ms/core/envvca/Tables.h"
#include "CoreModules/4ms/core/envvca/TriangleOscillator.h"
#include "CoreModules/4ms/core/helpers/EdgeDetector.h"
#include "CoreModules/4ms/core/helpers/FlipFlop.h"
#include "CoreModules/4ms/core/helpers/circuit_elements.h"
#include "CoreModules/4ms/core/helpers/quantization.h"

namespace MetaModule
{

class ENVVCACore : public SmartCoreProcessor<ENVVCAInfo> {
	using Info = ENVVCAInfo;
	using ThisCore = ENVVCACore;
	using enum Info::Elem;

public:
	ENVVCACore()
		: triggerDetector(1.0f, 2.0f) {
	}

	void update() override {
		auto [riseCV, fallCV] = getRiseAndFallCV();

		// Convert voltage to time without dealing with details of transistor core
		auto VoltageToTime = [](float voltage) -> float {
			auto frequency = VoltageToFrequencyTable.lookup(voltage);

			// limit to valid frequency range
			frequency = std::clamp(frequency, 1.0f / (60 * 3), 20e3f);

			// convert to period length
			auto time = 1.0f / frequency;

			return time;
		};

		osc.setRiseTimeInS(VoltageToTime(riseCV));
		osc.setFallTimeInS(VoltageToTime(fallCV));

		runOscillator();

		displayOscillatorState(osc.getSlopeState());

		displayEnvelope(osc.getOutput(), osc.getSlopeState());
		runAudioPath(osc.getOutput());
	}

	void runAudioPath(float triangleWave) {
		triangleWave = InvertingAmpWithBias(triangleWave, 100e3f, 100e3f, 1.94f);

		constexpr float VCAInputImpendance = 5e3f;
		triangleWave = triangleWave * VoltageDivider(VCAInputImpendance, 2.7e3f);

		// This value influences the maximum gain a lot
		// Tweaked manually to achieve approximately max 0dB
		constexpr float SchottkyForwardVoltage = 0.22f;
		constexpr float MaxGainInV = 5.0f + SchottkyForwardVoltage;
		constexpr float MinGainInV = VoltageDivider(47e3f, 1e6f) * 5.0f - SchottkyForwardVoltage;

		triangleWave = std::clamp(triangleWave, MinGainInV, MaxGainInV);

		vca.setScaling(triangleWave);

		// Ignoring input impedance and inverting 400kHz lowpass

		if (auto input = getInput<AudioIn>(); input) {
			auto output = vca.process(*input);
			setOutput<AudioOut>(output);
		} else {
			setOutput<AudioOut>(0.f);
		}

		// Ignoring output impedance and inverting 400kHz lowpass
	}

	void displayEnvelope(float val, TriangleOscillator::SlopeState_t slopeState) {
		setLED<RiseSlider>(slopeState == TriangleOscillator::SlopeState_t::RISING ? val / 8.f : 0);
		setLED<FallSlider>(slopeState == TriangleOscillator::SlopeState_t::FALLING ? val / 8.f : 0);
		val = val / VoltageDivider(100e3f, 100e3f);
		val *= getState<EnvLevelSlider>();
		setOutput<EnvOut>(val);
		setLED<EnvLevelSlider>(val / 8.f);
	}

	void displayOscillatorState(TriangleOscillator::SlopeState_t slopeState) {
		if (slopeState == TriangleOscillator::SlopeState_t::FALLING) {
			setOutput<EorOut>(8.f);
			setLED<EorLight>(true);
		} else {
			setOutput<EorOut>(0);
			setLED<EorLight>(false);
		}
	}

	void runOscillator() {
		bool isCycling =
			(getState<CycleButton>() == LatchingButton::State_t::DOWN) ^ CVToBool(getInput<CycleIn>().value_or(0.0f));

		osc.setCycling(isCycling);
		if (cycleLED != isCycling) {
			cycleLED = isCycling;
			setLED<CycleButton>(cycleLED);
		}

		if (auto inputFollowValue = getInput<FollowIn>(); inputFollowValue) {
			osc.setTargetVoltage(followInput.process(*inputFollowValue));
		}

		if (auto triggerInputValue = getInput<TriggerIn>(); triggerInputValue) {
			if (triggerEdgeDetector(triggerDetector(*triggerInputValue))) {
				osc.doRetrigger();
			}
		}

		osc.proceed(timeStepInS);
	}

	std::pair<float, float> getRiseAndFallCV() {
		auto ProcessCVOffset = [](auto slider, auto range) -> float {
			// Slider plus resistor in parallel to tweak curve
			const float SliderImpedance = 100e3f;
			auto offset = 5.0f * VoltageDivider(slider * SliderImpedance + 17.4e3f,
												0 + ParallelCircuit(100e3f, (1.0f - slider) * SliderImpedance));

			// Select one of three bias voltages
			auto BiasFromRange = [](auto range) -> float {
				if (range == Toggle3pos::State_t::UP) {
					return -12.0f * VoltageDivider(1e3f, 10e3f);
				} else if (range == Toggle3pos::State_t::DOWN) {
					return 12.0f * VoltageDivider(1e3f, 8.2e3f);
				} else {
					// middle position, and fail-safe default
					return 0.0f;
				}
			};

			auto bias = BiasFromRange(range);

			return InvertingAmpWithBias(offset, 100e3f, 100e3f, bias);
		};

		if (auto timeCVValue = getInput<TimeCvIn>(); timeCVValue) {
			// scale down cv input
			const auto scaledTimeCV = *timeCVValue * -100e3f / 137e3f;

			// apply attenuverter knobs
			rScaleLEDs = InvertingAmpWithBias(scaledTimeCV, 100e3f, 100e3f, getState<RiseCvKnob>() * scaledTimeCV);
			fScaleLEDs = InvertingAmpWithBias(scaledTimeCV, 100e3f, 100e3f, getState<FallCvKnob>() * scaledTimeCV);
		}

		// sum with static value from fader + range switch
		auto riseRange = getState<RiseSwitch>();
		auto fallRange = getState<FallSwitch>();
		riseCV = -rScaleLEDs - ProcessCVOffset(getState<RiseSlider>(), riseRange);
		fallCV = -fScaleLEDs - ProcessCVOffset(getState<FallSlider>(), fallRange);

		// TODO: LEDs only need to be updated ~60Hz instead of 48kHz
		setLED<RiseLight>(BipolarColor_t{-rScaleLEDs / 7.5f});
		setLED<FallLight>(BipolarColor_t{-fScaleLEDs / 7.5f});

		// TODO: low pass filter

		// apply rise time limit and scale down
		constexpr float DiodeDropInV = 1.0f;
		const float ClippingVoltage = 5.0f * VoltageDivider(100e3f, 2e3f) + DiodeDropInV;
		riseCV = riseCV * VoltageDivider(2.2e3f + 33e3f, 16.9e3f);
		riseCV = std::min(riseCV, ClippingVoltage);
		riseCV = riseCV * VoltageDivider(2.2e3f, 33e3f);

		// scale down falling CV without additional limiting
		fallCV = fallCV * VoltageDivider(2.2e3f, 10e3f + 40.2e3f);

		return {riseCV, fallCV};
	}

	void set_samplerate(float sr) override {
		timeStepInS = 1.0f / sr;
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	// temporary results that are buffered
	float cycleLED;
	float riseCV;
	float fallCV;
	float rScaleLEDs = 0.f;
	float fScaleLEDs = 0.f;

	FlipFlop triggerDetector;
	EdgeDetector triggerEdgeDetector;
	FollowInput followInput;

private:
	SSI2162 vca;
	TriangleOscillator osc;

private:
	float timeStepInS = 1.f / 48000.f;
};

} // namespace MetaModule
