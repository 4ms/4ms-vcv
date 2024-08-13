#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/LPF_info.hh"
#include "processors/lpf.h"
#include "processors/moogLadder.h"
#include "util/math.hh"
#include "util/math_tables.hh"

namespace MetaModule
{

class LPFCore : public CoreProcessor {
	using Info = LPFInfo;
	using ThisCore = LPFCore;

public:
	LPFCore() = default;

	void update() override {
	}

	void set_param(int param_id, float val) override {
		if (param_id == Info::KnobCutoff) {
			baseFrequency = map_value(val, 0.0f, 1.0f, -1.0f, 1.0f);
		} else if (param_id == Info::KnobQ) {
			filterQ = map_value(val, 0.0f, 1.0f, 1.0f, 20.0f);
			// } else if (param_id == Info::KnobCVAmount) {
			// 	cvAmount = val;
		} else if (param_id == static_cast<unsigned>(Info::SwitchMode) + static_cast<unsigned>(Info::NumKnobs)) {
			mode = val;
		}
	}

	void set_input(int input_id, float val) override {
		if (input_id == Info::InputInput)
			signalIn = val;
		if (input_id == Info::InputCv)
			cvInput = val / CvRangeVolts;
	}

	float get_output(int output_id) const override {
		if (output_id == Info::OutputOut)
			return signalOut;
		return 0.f;
	}

	void set_samplerate(float sr) override {
		lpf.sampleRate.setValue(sr);
	}

	float get_led_brightness(int led_id) const override {
		return mode;
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	int mode = 1;
	MoogLadder moog;
	LowPassFilter lpf;
	float signalIn = 0;
	float signalOut = 0;
	float baseFrequency = 1.0;
	float cvInput = 0;
	float filterQ = 0;
	// float cvAmount = 0;
};

} // namespace MetaModule
