#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Atvert2_info.hh"
#include "util/math.hh"

namespace MetaModule
{

class Atvert2Core : public CoreProcessor {
	using Info = Atvert2Info;
	using ThisCore = Atvert2Core;

public:
	Atvert2Core() = default;

	void update(void) override {
		out1 = (in1Connected ? in1 : defaultVoltage) * level1;
		out2 = (in2Connected ? in2 : defaultVoltage) * level2;
	}

	void set_param(int param_id, float val) override {
		float bipolarKnob = MathTools::map_value(val, 0.0f, 1.0f, -1.0f, 1.0f);
		switch (param_id) {
			case Info::Knob_1:
				level1 = bipolarKnob;
				break;
			case Info::Knob_2:
				level2 = bipolarKnob;
				break;
		}
	}

	void set_samplerate(const float sr) override {
	}

	void set_input(int input_id, float val) override {
		switch (input_id) {
			case Info::InputIn_1:
				in1 = val;
				break;
			case Info::InputIn_2:
				in2 = val;
				break;
		}
	}

	float get_output(int output_id) const override {
		switch (output_id) {
			case Info::OutputOut_1:
				return out1;
				break;
			case Info::OutputOut_2:
				return out2;
				break;
			default:
				return 0;
				break;
		}
	}

	void mark_input_unpatched(int input_id) override {
		if (input_id == Info::InputIn_1)
			in1Connected = false;
		else if (input_id == Info::InputIn_2)
			in2Connected = false;
	}

	void mark_input_patched(int input_id) override {
		if (input_id == Info::InputIn_1)
			in1Connected = true;
		else if (input_id == Info::InputIn_2)
			in2Connected = true;
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	float in1 = 0;
	float in2 = 0;
	float out1 = 0;
	float out2 = 0;
	float level1 = 0;
	float level2 = 0;

	bool in1Connected = false;
	bool in2Connected = false;

	static constexpr float defaultVoltage = 5.f;
};

} // namespace MetaModule
