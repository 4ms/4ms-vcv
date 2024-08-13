#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Switch41_info.hh"
#include "processors/tools/clockPhase.h"
#include "util/math.hh"

namespace MetaModule
{

class Switch41Core : public CoreProcessor {
	using Info = Switch41Info;
	using ThisCore = Switch41Core;

public:
	Switch41Core() = default;

	void update() override {
		cp.update();
		stepNum = cp.getCount() % 4;

		float position = cvInput * 3.0f;
		float fade = position - (int)position;

		switch ((int)position) {
			case 0:
				scanLevels[0] = 1.0f - fade;
				scanLevels[1] = fade;
				scanLevels[2] = 0;
				scanLevels[3] = 0;
				break;
			case 1:
				scanLevels[1] = 1.0f - fade;
				scanLevels[2] = fade;
				scanLevels[0] = 0;
				scanLevels[3] = 0;
				break;
			case 2:
				scanLevels[2] = 1.0f - fade;
				scanLevels[3] = fade;
				scanLevels[0] = 0;
				scanLevels[1] = 0;
				break;
			case 3:
				scanLevels[0] = 0;
				scanLevels[1] = 0;
				scanLevels[2] = 0;
				scanLevels[3] = 1.0f;
				break;
		}
	}

	void set_param(int param_id, float val) override {
	}

	void set_input(int input_id, float val) override {
		if (input_id == Info::InputClock) {
			cp.updateClock(val);
		} else if (input_id == Info::InputReset) {
			cp.updateReset(val);
		} else if (input_id == Info::InputCv) {
			cvInput = MathTools::constrain(val / CvRangeVolts, 0.0f, 1.0f);
		} else if (input_id >= Info::InputIn_1 && input_id <= Info::InputIn_4) {
			auto inputNum = input_id - Info::InputIn_1;
			signalInputs[inputNum] = val;
		}
	}

	float get_output(int output_id) const override {
		if (output_id != Info::OutputOut)
			return 0;

		float output = 0.0f;
		if (cvMode) {
			for (int i = 0; i < 4; i++) {
				output += signalInputs[i] * scanLevels[i];
			}
		} else {
			output = signalInputs[stepNum];
		}
		return output;
	}

	void set_samplerate(float sr) override {
	}

	float get_led_brightness(int led_id) const override {
		return 0.f;
	}
	void mark_input_unpatched(const int input_id) override {
		if (input_id == Info::InputCv) {
			cvMode = false;
		}
	}
	void mark_input_patched(const int input_id) override {
		if (input_id == Info::InputCv) {
			cvMode = true;
		}
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	int stepNum = 0;
	float signalInputs[4] = {0, 0, 0, 0};
	float cvInput = 0.0f;
	float scanLevels[4] = {0, 0, 0, 0};
	bool cvMode = false;
	ClockPhase cp;
};

} // namespace MetaModule
