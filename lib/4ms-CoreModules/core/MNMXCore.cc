#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/MNMX_info.hh"
#include "util/math.hh"

namespace MetaModule
{

class MNMXCore : public CoreProcessor {
	using Info = MNMXInfo;
	using ThisCore = MNMXCore;

public:
	MNMXCore() = default;

	void update() override {
	}

	void set_param(int param_id, float val) override {
	}

	void set_input(int input_id, float val) override {
		if (input_id == Info::InputIn_A)
			inA = val;

		if (input_id == Info::InputIn_B)
			inB = val;
	}

	float get_output(int output_id) const override {
		if (output_id == Info::OutputMin)
			return MathTools::min<float>(inA, inB);

		if (output_id == Info::OutputMax)
			return MathTools::max<float>(inA, inB);

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
	float inA = 0;
	float inB = 0;
};

} // namespace MetaModule
