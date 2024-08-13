#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "helpers/EdgeDetector.h"
#include "helpers/FlipFlop.h"
#include "info/SH_info.hh"

namespace MetaModule
{

class SHCore : public CoreProcessor {
	using Info = SHInfo;
	using ThisCore = SHCore;

public:
	SHCore()
		: triggerDetector{{0.1f, 0.2f}, {0.1f, 0.2f}} {
	}

	void update() override {
	}

	void set_param(int param_id, float val) override {
	}

	void set_input(int input_id, float val) override {
		switch (input_id) {
			case Info::InputIn_1:
				input[0] = val;
				break;

			case Info::InputSamp_1:
				if (trig[0](triggerDetector[0](val))) {
					held[0] = input[0];
				}
				break;

			case Info::InputIn_2:
				input[1] = val;
				break;

			case Info::InputSamp_2:
				if (trig[1](triggerDetector[1](val))) {
					held[1] = input[1];
				}
				break;
		}
	}

	float get_output(int output_id) const override {
		if (output_id < 0 || output_id > 1)
			return 0;

		return held[output_id];
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
	float held[2]{};
	float input[2]{};

	FlipFlop triggerDetector[2];
	EdgeDetector trig[2];
};

} // namespace MetaModule
