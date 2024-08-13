#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/BPF_info.hh"
#include "processors/bpf.h"
#include "processors/oberheimBPF.h"
#include "util/math.hh"

namespace MetaModule
{

class BPFCore : public CoreProcessor {
	using Info = BPFInfo;
	using ThisCore = BPFCore;

public:
	BPFCore() = default;

	void update(void) override {
		float filterFreq = 523.25f * setPitchMultiple(constrain(cutoffCV + cutoffOffset, -1.0f, 1.0f));
		if (mode == 0) {
			bpf.q = map_value(filterQ, 0.0f, 1.0f, 1.0f, 20.0f);
			bpf.cutoff.setValue(filterFreq);
			signalOutput = bpf.update(signalInput);
		} else if (mode == 1) {
			ober.q = map_value(filterQ, 0.0f, 1.0f, 1.0f, 20.0f);
			ober.cutoff.setValue(audioFreqToNorm(filterFreq));
			signalOutput = ober.update(signalInput);
		}
	}

	void set_param(int const param_id, const float val) override {
		switch (param_id) {
			case Info::KnobCutoff:
				cutoffOffset = map_value(val, 0.0f, 1.0f, -1.0f, 1.0f);
				break;
			case Info::KnobQ:
				filterQ = val;
				break;
			case (static_cast<unsigned>(Info::SwitchMode) + static_cast<unsigned>(Info::NumKnobs)):
				mode = val > 0.5f ? 1 : 0;
				break;
		}
	}

	void set_samplerate(const float sr) override {
		bpf.sampleRate.setValue(sr);
	}

	void set_input(const int input_id, const float val) override {
		switch (input_id) {
			case Info::InputInput:
				signalInput = val;
				break;
			case Info::InputCv:
				cutoffCV = val / CvRangeVolts;
				break;
		}
	}

	float get_output(const int output_id) const override {
		return signalOutput;
	}

	float get_led_brightness(const int led_id) const override {
		return mode;
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	int mode = 0;
	float filterQ = 1;
	BandPassFilter bpf;
	OberBPF ober;
	float cutoffCV = 0;
	float cutoffOffset = 0;
	float signalInput = 0;
	float signalOutput = 0;
};

} // namespace MetaModule
