#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Gate_info.hh"

#include "processors/tools/delayLine.h"
#include "processors/tools/schmittTrigger.h"
#include "util/math.hh"

namespace MetaModule
{

class GateCore : public CoreProcessor {
	using Info = GateInfo;
	using ThisCore = GateCore;

public:
	GateCore() = default;

	void update() override {
		lastGate = currentGate;
		currentGate = wc.output();
		if (currentGate && !lastGate) {
			finalDelay =
				MathTools::map_value(MathTools::constrain(delayCV + delayTime, 0.0f, 1.0f), 0.0f, 1.0f, 0.0f, 1000.0f) /
				1000.0f * sampleRate;
			finalLength = MathTools::map_value(
							  MathTools::constrain(lengthCV + gateLength, 0.0f, 1.0f), 0.0f, 1.0f, 1.0f, 1000.0f) /
						  1000.0f * sampleRate;

			del.set_delay_samples(finalDelay);

			sinceGate = 0;
		}

		wc.update(del.update(signalInput));

		gateOutput = (sinceGate < finalLength);
		sinceGate++;
	}

	void set_param(int param_id, float val) override {
		switch (param_id) {
			case Info::KnobLength:
				gateLength = val;
				break;
			case Info::KnobDelay:
				delayTime = val;
				break;
		}
	}

	void set_input(int input_id, float val) override {
		switch (input_id) {
			case Info::InputInput:
				signalInput = val;
				break;
			case Info::InputLength_Cv:
				lengthCV = val / CvRangeVolts;
				break;
			case Info::InputDelay_Cv:
				delayCV = val / CvRangeVolts;
				break;
		}
	}

	float get_output(int output_id) const override {
		if (output_id == Info::OutputOut)
			return gateOutput ? MaxOutputVolts : 0.f;
		return 0.f;
	}

	void set_samplerate(float sr) override {
		sampleRate = sr;
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
	float sampleRate = 48000;
	bool lastGate = false;
	bool currentGate = false;
	bool gateOutput = false;

	unsigned long sinceGate = 0;

	float gateLength = 10;

	float delayTime = 0;

	float signalInput = 0;

	float lengthCV = 0;
	float delayCV = 0;

	float finalLength = 10;
	float finalDelay = 0;

	DelayLine<96000> del;
	SchmittTrigger wc;
};

} // namespace MetaModule
