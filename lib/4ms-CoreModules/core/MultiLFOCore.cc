#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "core/helpers/EdgeDetector.h"
#include "info/MultiLFO_info.hh"
#include "processors/tools/schmittTrigger.h"
#include "util/math.hh"
#include "util/math_tables.hh"

namespace MetaModule
{

class MultiLFOCore : public CoreProcessor {
	using Info = MultiLFOInfo;
	using ThisCore = MultiLFOCore;

public:
	MultiLFOCore() = default;

	void update() override {
		if (rateChanged) {
			combineKnobCVFreq();
			rateChanged = false;
		}

		phaccu += finalRate / sampRate;
		if (phaccu >= 1.0f)
			phaccu -= 1.0f;
		modPhase = phaccu + phaseOffset;
		if (modPhase >= 1.0f)
			modPhase -= 1.0f;
	}

	void combineKnobCVFreq() {
		auto knobFreq = exp5Table.closest(MathTools::constrain(rawRateKnob, 0.f, 1.f));
		finalRate = knobFreq * MathTools::setPitchMultiple(rawRateCV);
	}

	void set_param(int param_id, float val) override {
		switch (param_id) {
			case Info::KnobRate:
				rawRateKnob = val;
				rateChanged = true;
				break;
			case Info::KnobPhase:
				phaseOffset = val;
				break;
			case Info::KnobPw:
				pwOffset = val;
				break;
		}
	}

	void set_input(int input_id, float val) override {
		val = val / CvRangeVolts;

		switch (input_id) {
			case Info::InputRate_Cv:
				rawRateCV = val;
				rateChanged = true;
				break;
			case Info::InputPhase_Cv:
				phaseCV = val;
				break;
			case Info::InputPw_Cv:
				pwCV = val;
				break;
			case Info::InputReset:
				if (reset_edge(reset.update(val)))
					phaccu = 0;
				break;
		}
	}

	float get_output(int output_id) const override {
		switch (output_id) {
			case Info::OutputSine:
				return sinTable.interp_wrap(modPhase) * MaxOutputVolts;
				break;
			case Info::OutputSaw:
				return (modPhase * 2.f - 1.f) * MaxOutputVolts;
				break;
			case Info::OutputInv_Saw:
				return (1.f - modPhase * 2.f) * MaxOutputVolts;
				break;
			case Info::OutputPulse:
				return (modPhase < (pwOffset + pwCV)) ? MaxOutputVolts : -MaxOutputVolts;
				break;
		}
		return 0.f;
	}

	void set_samplerate(float sr) override {
		sampRate = sr;
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
	bool rateChanged = true;
	float phaccu = 0.f;
	float finalRate = 0.1f;
	float sampRate = 48000.f;
	float rawRateKnob = 0.0f;
	float rawRateCV = 0.0f;
	float phaseCV = 0.f;
	float pwOffset = 0.5f;
	float pwCV = 0.f;
	float modPhase = 0.f;
	float phaseOffset = 0.f;

	// bool currentReset = 0.f;
	// bool lastReset = 0.f;

	SchmittTrigger reset;
	EdgeDetector reset_edge;
};

} // namespace MetaModule
