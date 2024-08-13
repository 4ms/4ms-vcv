#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/PitchShift_info.hh"
#include "processors/pitchShift.h"
#include "util/math.hh"

namespace MetaModule
{

class PitchShiftCore : public CoreProcessor {
	using Info = PitchShiftInfo;
	using ThisCore = PitchShiftCore;

public:
	PitchShiftCore() = default;

	void update() override {
		auto finalWindow = MathTools::constrain(windowOffset + windowCV, 0.0f, 1.0f);
		p.windowSize = MathTools::map_value(finalWindow, 0.0f, 1.0f, 20.0f, static_cast<float>(maxWindowSize));
		p.shiftAmount = coarseShift + fineShift + shiftCV * 12.f;
		p.mix = MathTools::constrain(mixOffset + mixCV, 0.0f, 1.0f);
		signalOutput = p.update(signalInput);
	}

	void set_param(int param_id, float val) override {
		switch (param_id) {
			case Info::KnobCoarse:
				coarseShift = MathTools::map_value(val, 0.0f, 1.0f, -12.0f, 12.0f);
				break;
			case Info::KnobFine:
				fineShift = MathTools::map_value(val, 0.0f, 1.0f, -1.0f, 1.0f);
				break;
			case Info::KnobWindow:
				windowOffset = val;
				break;
			case Info::KnobMix:
				mixOffset = val;
				break;
		}
	}

	void set_input(int input_id, float val) override {
		switch (input_id) {
			case Info::InputInput:
				signalInput = val;
				break;
			case Info::InputPitch_Cv:
				shiftCV = MathTools::constrain(val, -5.f, +5.f);
				break;
			case Info::InputWindow_Cv:
				windowCV = val / CvRangeVolts;
				break;
			case Info::InputMix_Cv:
				mixCV = val / CvRangeVolts;
				break;
		}
	}

	float get_output(int output_id) const override {
		return signalOutput;
	}

	void set_samplerate(float sr) override {
		p.setSampleRate(sr);
	}

	float get_led_brightness(int led_id) const override {
		return 0.f;
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	const static inline long maxWindowSize = 9600;
	PitchShift<maxWindowSize> p;

	float signalInput = 0;
	float shiftCV = 0;
	float windowCV = 0;
	float mixCV = 0;
	float signalOutput = 0;
	float coarseShift = 0;
	float fineShift = 0;
	float mixOffset = 0;
	float windowOffset = 100;
};

} // namespace MetaModule
