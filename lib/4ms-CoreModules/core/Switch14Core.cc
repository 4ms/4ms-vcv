#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Switch14_info.hh"
#include "processors/tools/clockPhase.h"
#include "util/math.hh"

namespace MetaModule
{

class Switch14Core : public CoreProcessor {
	using Info = Switch14Info;
	using ThisCore = Switch14Core;

public:
	Switch14Core() = default;

	void update() override {
		cp.update();
		stepNum = cp.getCount() % NumThrows;

		if (cvMode) {
			float position = cvSignal * 3.0f;
			float fade = position - (int)position;

			switch ((int)position) {
				case 0:
					panSignals[0] = 1.0f - fade;
					panSignals[1] = fade;
					panSignals[2] = 0;
					panSignals[3] = 0;
					break;
				case 1:
					panSignals[0] = 0;
					panSignals[1] = 1.0f - fade;
					panSignals[2] = fade;
					panSignals[3] = 0;
					break;
				case 2:
					panSignals[0] = 0;
					panSignals[1] = 0;
					panSignals[2] = 1.0f - fade;
					panSignals[3] = fade;
					break;
				case 3:
					panSignals[0] = 0;
					panSignals[1] = 0;
					panSignals[2] = 0;
					panSignals[3] = 1.0f;
					break;
			}
		}
	}

	void set_param(int param_id, float val) override {
	}

	void set_input(int input_id, float val) override {
		switch (input_id) {
			case Info::InputClock: // clock
				cp.updateClock(val / CvRangeVolts);
				break;
			case Info::InputReset: // reset
				cp.updateReset(val / CvRangeVolts);
				break;
			case Info::InputIn: // signal
				inputSignal = val;
				break;
			case Info::InputCv:
				cvSignal = MathTools::constrain(val / CvRangeVolts, 0.0f, 1.0f);
				break;
		}
	}

	float get_output(int output_id) const override {
		// Output jacks must be sequential
		// or else our logic doesn't work:
		static_assert(Info::OutputOut_1 + 1 == Info::OutputOut_2);
		static_assert(Info::OutputOut_2 + 1 == Info::OutputOut_3);
		static_assert(Info::OutputOut_3 + 1 == Info::OutputOut_4);

		if (output_id < (int)NumThrows) {
			if (cvMode)
				return panSignals[output_id] * inputSignal;

			if (output_id == stepNum + Info::OutputOut_1)
				return inputSignal;
		}
		return 0;
	}

	void set_samplerate(float sr) override {
	}

	float get_led_brightness(int led_id) const override {
		return 0.f;
	}

	void mark_input_unpatched(const int input_id) override {
		if (input_id == Info::InputCv)
			cvMode = false;
	}

	void mark_input_patched(const int input_id) override {
		if (input_id == Info::InputCv)
			cvMode = true;
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	static constexpr size_t NumThrows = 4;
	ClockPhase cp;
	float panSignals[NumThrows]{};
	float cvSignal = 0;
	bool cvMode = false;
	int stepNum = 0;
	float inputSignal = 0;
};

} // namespace MetaModule
