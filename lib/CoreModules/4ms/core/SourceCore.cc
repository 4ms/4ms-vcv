#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Source_info.hh"
#include "util/math.hh"

namespace MetaModule
{

class SourceCore : public CoreProcessor {
	using Info = SourceInfo;
	using ThisCore = SourceCore;

public:
	SourceCore() = default;

	void update(void) override {
	}

	void set_param(int const param_id, const float val) override {
		switch (param_id) {
			case Info::Knob_1:
				output1 = MathTools::map_value(val, 0.0f, 1.0f, -1.0f, 1.0f);
				break;
			case Info::Knob_2:
				output2 = MathTools::map_value(val, 0.0f, 1.0f, -1.0f, 1.0f);
				break;
			case 3: //hidden param used for calibration checking
				octave_mode = (val + (octave_mode ? 0.1 : -0.1)) > 0.5f;
		}
	}

	void set_samplerate(const float sr) override {
	}

	void set_input(const int input_id, const float val) override {
	}

	float get_output(const int output_id) const override {
		switch (output_id) {
			case Info::Output_1_Out: {
				float out = output1 * OutputVoltageRange;
				return octave_mode ? std::round(out) : out;
			} break;

			case Info::Output_2_Out: {
				float out = output2 * OutputVoltageRange;
				return octave_mode ? std::round(out) : out;
			} break;
		}
		return 0;
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
	float output1{};
	float output2{};
	bool octave_mode = false;

	static constexpr float OutputVoltageRange = 10.f;
};

} // namespace MetaModule
