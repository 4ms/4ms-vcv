#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/CLKM_info.hh"
#include "processors/tools/clockPhase.h"
#include "util/math.hh"

using namespace MathTools;

namespace MetaModule
{

class CLKMCore : public CoreProcessor {
	using Info = CLKMInfo;
	using ThisCore = CLKMCore;

public:
	CLKMCore() = default;

	void update() override {
		float finalMultiply = constrain(multiplyOffset + multiplyCV, 0.0f, 1.0f);
		cp.setMultiply(map_value(finalMultiply, 0.0f, 1.0f, 1.0f, 16.99f));
		cp.update();
		if (cp.getWrappedPhase() < pulseWidth) {
			clockOutput = gateVoltage;
		} else {
			clockOutput = 0;
		}
	}

	void set_param(int param_id, float val) override {
		switch (param_id) {
			case Info::KnobMultiply:
				multiplyOffset = val;
				break;
		}
	}

	void set_input(int input_id, float val) override {
		switch (input_id) {
			case Info::InputClk_In:
				cp.updateClock(val);
				break;
			case Info::InputCv:
				multiplyCV = val / CvRangeVolts;
		}
	}

	float get_output(int output_id) const override {
		return clockOutput;
	}

	void set_samplerate(float sr) override {
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
	float pulseWidth = 0.5f;
	int clockOutput = 0;
	float multiplyOffset = 0;
	float multiplyCV = 0;

	ClockPhase cp;

	static constexpr float gateVoltage = 8.0f;
};

} // namespace MetaModule
