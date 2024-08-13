#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Slew_info.hh"
#include "processors/tools/expDecay.h"
#include "util/math.hh"

namespace MetaModule
{

class SlewCore : public CoreProcessor {
	using Info = SlewInfo;
	using ThisCore = SlewCore;

public:
	SlewCore() = default;

	void update() override {
		signalOutput = slew.update(signalInput);
	}

	void set_param(int param_id, float val) override {
		if (val < 0)
			val = 0.f;

		switch (param_id) {
			case Info::KnobRise:
				slew.attackTime = MathTools::map_value(val, 0.0f, CvRangeVolts, 1.0f, 2000.0f);
				break;
			case Info::KnobFall:
				slew.decayTime = MathTools::map_value(val, 0.0f, CvRangeVolts, 1.0f, 2000.0f);
				break;
		}
	}

	void set_input(int input_id, float val) override {
		if (input_id == Info::InputInput)
			signalInput = val;
	}

	float get_output(int output_id) const override {
		if (output_id == Info::OutputOut)
			return signalOutput;
		return 0.f;
	}

	void set_samplerate(float sr) override {
		slew.set_samplerate(sr);
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
	ExpDecay slew;
	float signalInput = 0;
	float signalOutput = 0;
};

} // namespace MetaModule
