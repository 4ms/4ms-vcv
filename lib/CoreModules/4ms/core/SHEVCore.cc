#include "info/SHEV_info.hh"
#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"

#include "CoreModules/4ms/core/envvca/SSI2162.h"
#include "CoreModules/4ms/core/envvca/TriangleOscillator.h"
#include "CoreModules/4ms/core/envvca/Tables.h"
#include "CoreModules/4ms/core/envvca/FollowInput.h"
#include "CoreModules/4ms/core/helpers/EdgeDetector.h"
#include "CoreModules/4ms/core/helpers/circuit_elements.h"
#include "CoreModules/4ms/core/helpers/FlipFlop.h"
#include "CoreModules/4ms/core/helpers/quantization.h"


namespace MetaModule
{

struct LogTableRange {
	static constexpr float min = 0.0f;
	static constexpr float max = 5.0f;
};

constinit auto LogTableSHEV =
	Mapping::LookupTable_t<50>::generate<LogTableRange>([](auto voltage) {
		return gcem::log(29.6f * voltage + 1.0f);
	});

struct ExpTableRange {
	static constexpr float min = 0.0f;
	static constexpr float max = 5.0f;
};

constinit auto ExpTableSHEV =
	Mapping::LookupTable_t<50>::generate<ExpTableRange>([](auto voltage) {
		return (gcem::exp(voltage) - 1.0f) / 29.682631f;
	});


class SHEVCore : public SmartCoreProcessor<SHEVInfo> {
	using Info = SHEVInfo;
	using ThisCore = SHEVCore;
	using enum Info::Elem;

public:
	template<Info::Elem EL>
	void setOutput(auto val)
	{
		return SmartCoreProcessor<Info>::setOutput<EL>(val);
	}

	template<Info::Elem EL>
	auto getInput()
	{
		return SmartCoreProcessor<Info>::getInput<EL>();
	}

	template<Info::Elem EL, typename VAL>
	void setLED(const VAL &value)
	{
		return SmartCoreProcessor<Info>::setLED<EL>(value);
	}

	template<Info::Elem EL>
	auto getState()
	{
		return SmartCoreProcessor<Info>::getState<EL>();
	}

private:
	template <class Mapping>
	class Channel
	{
	private:
		SSI2162 vca;
		TriangleOscillator osc;

		FlipFlop triggerDetector;
		EdgeDetector triggerEdgeDetector;

		FollowInput followInput;

	private:
		float cycleLED;
		float riseCV;
		float fallCV;
		float rScaleLEDs = 0.f;
		float fScaleLEDs = 0.f;
		float envOut;
		enum TriggerMode_t {CYCLE, ASR, AR};
		TriggerMode_t triggerMode;

	private:
		float timeStepInS = 1.f / 48000.f;

	private:
		SHEVCore* parent;

	private:
		static constexpr float followInputHysteresisInV = 0.01f;
		float previousFollowInputValue;

		static constexpr float followInputFilterCoeff = 0.01f;
		float previousFollowInputFilterOutput;

	public:
		Channel(SHEVCore* parent_)
			: triggerDetector(1.0f, 2.0f), parent(parent_){
		}

		void update(auto input) {
			auto [riseCV, fallCV] = getRiseAndFallCV();

			osc.setRiseTimeInS(VoltageToTime(riseCV));
			osc.setFallTimeInS(VoltageToTime(fallCV));

			runOscillator();

			auto shapedEnv = shapeEnvelope(osc.getOutput());

			displayEnvelope(shapedEnv, osc.getOutput(), osc.getSlopeState());

			if(auto vcaCV = parent->getInput<Mapping::VcaCvIn>(); vcaCV) {
				runAudioPath(*vcaCV, input);
			} else {
				runAudioPath(shapedEnv, input);
			}		
			
		}

		void runAudioPath(float triangleWave, auto input) {
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

			if (input) {
				auto output = vca.process(*input);
				parent->setOutput<Mapping::AudioOut>(output);
			} else {
				parent->setOutput<Mapping::AudioOut>(0.f);
			}
		}

		auto shapeEnvelope(float val)
		{
			auto shapeControl = parent->getState<Mapping::ShapeSlider>();

			if(auto shapeCV = parent->getInput<Mapping::ShapeCVIn>(); shapeCV) {
				shapeControl += (parent->getState<Mapping::ShapeKnob>() * 2.0f - 1.0f) * *shapeCV / 10.0f;
				shapeControl = std::clamp(shapeControl, 0.0f, 1.0f);
			}
			parent->setLED<Mapping::ShapeLight>(FullColor_t{shapeControl});

			float shapedEnv;

			if(shapeControl > 0.5f) {
				auto mixFactor = 2.0f * shapeControl - 1.0f;
				shapedEnv = (1.0f - mixFactor) * val + mixFactor * LogTableSHEV.lookup(val);
			} else {
				auto mixFactor = 2.0f * shapeControl;
				shapedEnv = (1.0f - mixFactor) * ExpTableSHEV.lookup(val) + mixFactor * val;
			}

			return shapedEnv;
		}

		void displayEnvelope(float shapedEnv, float val, TriangleOscillator::SlopeState_t slopeState) {
			parent->setLED<Mapping::RiseSlider>(slopeState == TriangleOscillator::SlopeState_t::RISING ? val / 8.f : 0);
			parent->setLED<Mapping::FallSlider>(slopeState == TriangleOscillator::SlopeState_t::FALLING ? val / 8.f : 0);
			parent->setLED<Mapping::ShapeSlider>(val / 8.f);

			envOut = shapedEnv / VoltageDivider(100e3f, 100e3f);
			envOut *= parent->getState<Mapping::LevelKnob>() * 2.0f - 1.0f;
			envOut += parent->getState<Mapping::OffsetKnob>() * 20.0f - 10.0f;
			parent->setOutput<Mapping::EnvOut>(envOut);
			parent->setOutput<Mapping::Lin5VOut>(val);
			parent->setLED<Mapping::EnvLedLight>(BipolarColor_t{envOut / 8.f});
		}

		auto getEnvOut() {
			return envOut;
		}

		void runOscillator() {		
			auto toggleState = parent->getState<Mapping::CycleArAsrSwitch>();

			if (toggleState == Toggle3pos::State_t::UP) {
				triggerMode = TriggerMode_t::CYCLE;
			} else if (toggleState == Toggle3pos::State_t::DOWN) {
				triggerMode = TriggerMode_t::ASR;
			} else {
				triggerMode = TriggerMode_t::AR;
			}

			auto triggerInputValue = parent->getInput<Mapping::TrigIn>();

			bool isCycling;

			if(triggerMode == TriggerMode_t::CYCLE) {
				isCycling = (parent->getState<Mapping::CycleButton>() == LatchingButton::State_t::DOWN) ^ CVToBool(triggerInputValue.value_or(0.0f));
			} else {
				isCycling = (parent->getState<Mapping::CycleButton>() == LatchingButton::State_t::DOWN);
			};

			osc.setCycling(isCycling);

			if (cycleLED != isCycling) {
				cycleLED = isCycling;
				parent->setLED<Mapping::CycleButton>(cycleLED);
			}

			if(triggerInputValue) {
				if (triggerEdgeDetector(triggerDetector(*triggerInputValue))) {
					osc.doRetrigger();
				}
			}

			if(triggerMode == TriggerMode_t::ASR && triggerDetector(*triggerInputValue)) {
				osc.holdMax(true);
			} else {
				osc.holdMax(false);
			}

			if (auto inputFollowValue = parent->getInput<Mapping::FollowIn>(); inputFollowValue) {
				osc.setTargetVoltage(followInput.process(*inputFollowValue));
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

			if (auto timeCVValue = parent->getInput<Mapping::TimeCvIn>(); timeCVValue) {
				// scale down cv input
				const auto scaledTimeCV = *timeCVValue * -100e3f / 137e3f;

				// apply attenuverter knobs
				rScaleLEDs = InvertingAmpWithBias(scaledTimeCV, 100e3f, 100e3f, parent->getState<Mapping::RiseKnob>() * scaledTimeCV);
				fScaleLEDs = InvertingAmpWithBias(scaledTimeCV, 100e3f, 100e3f, parent->getState<Mapping::FallKnob>() * scaledTimeCV);
			}

			// sum with static value from fader + range switch
			auto riseRange = parent->getState<Mapping::SlowMedFastRiseSwitch>();
			auto fallRange = parent->getState<Mapping::SlowMedFastFallSwitch>();
			riseCV = -rScaleLEDs - ProcessCVOffset(parent->getState<Mapping::RiseSlider>(), riseRange);
			fallCV = -fScaleLEDs - ProcessCVOffset(parent->getState<Mapping::FallSlider>(), fallRange);

			// TODO: LEDs only need to be updated ~60Hz instead of 48kHz
			parent->setLED<Mapping::RiseLedLight>(BipolarColor_t{-rScaleLEDs / 10.f});
			parent->setLED<Mapping::FallLedLight>(BipolarColor_t{-fScaleLEDs / 10.f});

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

		TriangleOscillator::SlopeState_t getOscillatorSlopeState()
		{
			return osc.getSlopeState();
		}

		void set_samplerate(float sr) {
			timeStepInS = 1.0f / sr;
		}

	};

private:
	struct MappingA
	{
		const static Info::Elem AudioIn = AudioAIn;
		const static Info::Elem AudioOut = OutAOut;
		const static Info::Elem RiseSlider = RiseASlider;
		const static Info::Elem FallSlider = FallASlider;
		const static Info::Elem EnvOut = EnvAOut;
		const static Info::Elem EnvLedLight = EnvALight;
		const static Info::Elem LevelKnob = LevelAKnob;
		const static Info::Elem OffsetKnob = OffsetAKnob;
		const static Info::Elem CycleButton = CycleAButton;
		const static Info::Elem FollowIn = FollowAIn;
		const static Info::Elem TrigIn = TrigAIn;
		const static Info::Elem RiseKnob = RiseAKnob;
		const static Info::Elem FallKnob = FallAKnob;
		const static Info::Elem TimeCvIn = TimeCvAIn;
		const static Info::Elem SlowMedFastRiseSwitch = RiseASwitch;
		const static Info::Elem SlowMedFastFallSwitch = FallASwitch;
		const static Info::Elem RiseLedLight = RiseALight;
		const static Info::Elem FallLedLight = FallALight;
		const static Info::Elem VcaCvIn = VcaCvAIn;
		const static Info::Elem Lin5VOut = Lin5VAOut;
		const static Info::Elem ShapeCVIn = ShapeCvAIn;
		const static Info::Elem ShapeSlider = ShapeASlider;
		const static Info::Elem ShapeKnob = ShapeAKnob;
		const static Info::Elem CycleArAsrSwitch = TrigModeASwitch;
		const static Info::Elem ShapeLight = ShapeALight;
	};

	struct MappingB
	{
		const static Info::Elem AudioIn = AudioBIn;
		const static Info::Elem AudioOut = OutBOut;
		const static Info::Elem RiseSlider = RiseBSlider;
		const static Info::Elem FallSlider = FallBSlider;
		const static Info::Elem EnvOut = EnvBOut;
		const static Info::Elem EnvLedLight = EnvBLight;
		const static Info::Elem LevelKnob = LevelBKnob;
		const static Info::Elem OffsetKnob = OffsetBKnob;
		const static Info::Elem CycleButton = CycleBButton;
		const static Info::Elem FollowIn = FollowBIn;
		const static Info::Elem TrigIn = TrigBIn;
		const static Info::Elem RiseKnob = RiseBKnob;
		const static Info::Elem FallKnob = FallBKnob;
		const static Info::Elem TimeCvIn = TimeCvBIn;
		const static Info::Elem SlowMedFastRiseSwitch = RiseBSwitch;
		const static Info::Elem SlowMedFastFallSwitch = FallBSwitch;
		const static Info::Elem RiseLedLight = RiseBLight;
		const static Info::Elem FallLedLight = FallBLight;
		const static Info::Elem VcaCvIn = VcaCvBIn;
		const static Info::Elem Lin5VOut = Lin5VBOut;
		const static Info::Elem ShapeCVIn = ShapeCvBIn;
		const static Info::Elem ShapeSlider = ShapeBSlider;
		const static Info::Elem ShapeKnob = ShapeBKnob;
		const static Info::Elem CycleArAsrSwitch = TrigModeBSwitch;
		const static Info::Elem ShapeLight = ShapeBLight;
	};

	Channel<MappingA> channelA;
	Channel<MappingB> channelB;

friend Channel<MappingA>;
friend Channel<MappingB>;

public:
	SHEVCore()
		: channelA(this), channelB(this){
		
	}

	void update() override {

		auto inputA = getInput<MappingA::AudioIn>();
		channelA.update(inputA);
		
		if(auto inputB = getInput<MappingB::AudioIn>(); inputB) {
			channelB.update(inputB);
		} else {
			channelB.update(inputA);
		}
		
		displayOscillatorState();

		setOutput<OrOut>(std::max(channelA.getEnvOut(), channelB.getEnvOut()));
	}

	void displayOscillatorState()
	{
		if(auto slopeStateA = channelA.getOscillatorSlopeState(); slopeStateA == TriangleOscillator::SlopeState_t::FALLING) {
			setOutput<EorAOut>(8.f);
			setLED<EorALight>(true);
		} else {
			setOutput<EorAOut>(0);
			setLED<EorALight>(false);
		}

		if(auto slopeStateB = channelB.getOscillatorSlopeState(); slopeStateB != TriangleOscillator::SlopeState_t::FALLING) {
			setOutput<EofBOut>(8.f);
			setLED<EofBLight>(true);
		} else {
			setOutput<EofBOut>(0);
			setLED<EofBLight>(false);
		}
	}

	void set_samplerate(float sr) override {
		channelA.set_samplerate(sr);
		channelB.set_samplerate(sr);
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	

};

} // namespace MetaModule
