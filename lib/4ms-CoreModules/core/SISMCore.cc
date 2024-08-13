#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/SISM_info.hh"

namespace MetaModule
{

class SISMCore : public SmartCoreProcessor<SISMInfo> {
	using Info = SISMInfo;
	using ThisCore = SISMCore;
	using enum Info::Elem;

public:
	SISMCore()
		: outputValue{0.f,0.f,0.f,0.f}, inputValue{0.f,0.f,0.f,0.f} {
	}

	void update() override {
		if(auto channel1InputValue = getInput<In1In>(); channel1InputValue) {
			inputValue[0] = *channel1InputValue;
		} else {
			inputValue[0] = 0.f;
		}

		if(auto channel2InputValue = getInput<In2In>(); channel2InputValue) {
			inputValue[1] = *channel2InputValue;
		} else {
			inputValue[1] = inputValue[0];
		}

		if(auto channel3InputValue = getInput<In3In>(); channel3InputValue) {
			inputValue[2] = *channel3InputValue;
		} else {
			inputValue[2] = 0.f;
		}

		if(auto channel4InputValue = getInput<In4In>(); channel4InputValue) {
			inputValue[3] = *channel4InputValue;
		} else {
			inputValue[3] = inputValue[2];
		}

		outputValue[0] = process(inputValue[0], getState<Scale1Knob>(), getState<Shift1Knob>());
		outputValue[1] = process(inputValue[1], getState<Scale2Knob>(), getState<Shift2Knob>());
		outputValue[2] = process(inputValue[2], getState<Scale3Knob>(), getState<Shift3Knob>());
		outputValue[3] = process(inputValue[3], getState<Scale4Knob>(), getState<Shift4Knob>());

		setOutput<Out1Out>(std::clamp(outputValue[0], minimumOutputInV, maximumOutputInV));
		setOutput<Out2Out>(std::clamp(outputValue[1], minimumOutputInV, maximumOutputInV));
		setOutput<Out3Out>(std::clamp(outputValue[2], minimumOutputInV, maximumOutputInV));
		setOutput<Out4Out>(std::clamp(outputValue[3], minimumOutputInV, maximumOutputInV));

		setLED<LedN1Light>(outputValue[0] / negativeLEDScaling);
		setLED<LedP1Light>(outputValue[0] / positiveLEDScaling);

		setLED<LedN2Light>(outputValue[1] / negativeLEDScaling);
		setLED<LedP2Light>(outputValue[1] / positiveLEDScaling);

		setLED<LedN3Light>(outputValue[2] / negativeLEDScaling);
		setLED<LedP3Light>(outputValue[2] / positiveLEDScaling);

		setLED<LedN4Light>(outputValue[3] / negativeLEDScaling);
		setLED<LedP4Light>(outputValue[3] / positiveLEDScaling);

		auto slicePositive = 0.f;
		auto sliceNegative = 0.f;
		auto mixOut = 0.f;
		auto mixOutSW = 0.f;

		for (auto index = 0u; index < outputValue.size(); index++) {
			slicePositive += std::clamp(outputValue[index], 0.f, maximumOutputInV);
			sliceNegative += std::clamp(outputValue[index], minimumOutputInV, 0.f);
			mixOut += outputValue[index];

			mixOutSW += isPatched<Out1Out>() ? 0 : outputValue[0];
			mixOutSW += isPatched<Out2Out>() ? 0 : outputValue[1];
			mixOutSW += isPatched<Out3Out>() ? 0 : outputValue[2];
			mixOutSW += isPatched<Out4Out>() ? 0 : outputValue[3];
		}

		setOutput<PSliceOut>(std::clamp(slicePositive, 0.f, maximumOutputInV));
		setOutput<NSliceOut>(std::clamp(sliceNegative, minimumOutputInV, 0.f));

		setLED<LedPSliceLight>(slicePositive / positiveLEDScaling);
		setLED<LedNSliceLight>(sliceNegative / negativeLEDScaling);

		setOutput<MixOut>(std::clamp(mixOut, minimumOutputInV, maximumOutputInV));

		setLED<LedNMixLight>(mixOut / negativeLEDScaling);
		setLED<LedPMixLight>(mixOut / positiveLEDScaling);

		setOutput<Mix_Sw_Out>(std::clamp(mixOutSW, minimumOutputInV, maximumOutputInV));

		setLED<LedNMix_Sw_Light>(mixOutSW / negativeLEDScaling);
		setLED<LedPMix_Sw_Light>(mixOutSW / positiveLEDScaling);
	}

	void set_samplerate(float sr) override {
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	static constexpr float maximumShiftInV = 9.5f;
	static constexpr float maximumOutputInV = 10.f;
	static constexpr float minimumOutputInV = -10.f;
	static constexpr float negativeLEDScaling = -8.f;
	static constexpr float positiveLEDScaling = 8.f;
	std::array<float, 4> outputValue;
	std::array<float, 4> inputValue;

private:
	float process(float input, float scaleAmount, float shiftAmount) {
		return input * attenueverte(scaleAmount) + attenueverte(shiftAmount) * maximumShiftInV;
	}

	float attenueverte(float input) {
		return 2.0f * input - 1.0f;
	}
};

} // namespace MetaModule
