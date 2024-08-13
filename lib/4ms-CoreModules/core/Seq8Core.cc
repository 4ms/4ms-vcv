#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Seq8_info.hh"
#include "processors/stepsequencer.h"
#include "util/math.hh"

namespace MetaModule
{

class Seq8Core : public CoreProcessor {
	using Info = Seq8Info;
	using ThisCore = Seq8Core;

public:
	Seq8Core() = default;

	void update() override {
		seq.update();
	}

	void set_param(int param_id, float val) override {
		static_assert(Info::Knob_1 + 1 == Info::Knob_2);
		static_assert(Info::Knob_2 + 1 == Info::Knob_3);
		static_assert(Info::Knob_3 + 1 == Info::Knob_4);
		static_assert(Info::Knob_4 + 1 == Info::Knob_5);
		static_assert(Info::Knob_5 + 1 == Info::Knob_6);
		static_assert(Info::Knob_6 + 1 == Info::Knob_7);
		static_assert(Info::Knob_7 + 1 == Info::Knob_8);

		if (param_id >= Info::Knob_1 && param_id <= Info::Knob_8)
			seq.setStep(param_id - Info::Knob_1, val);
		// else if (param_id == Info::KnobLength)
		// 	seq.setLength(MathTools::map_value(val, 0.0f, 1.0f, 0, 8));
	}

	void set_input(int input_id, float val) override {
		switch (input_id) {
			case Info::InputClock:
				seq.updateClock(val);
				break;
			case Info::InputReset:
				seq.updateReset(val);
				break;
		}
	}

	float get_output(int output_id) const override {
		switch (output_id) {
			case Info::OutputGate:
				return seq.endOutput * GateOutVolts;

			case Info::OutputOut:
				return seq.output * CVOutVolts;
		}
		return 0.f;
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
	StepSequencer seq{8};

	static constexpr float GateOutVolts = 8.f;
	static constexpr float CVOutVolts = 10.f;
};

} // namespace MetaModule
