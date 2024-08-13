#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Noise_info.hh"
#include "util/math.hh"

namespace MetaModule
{

class NoiseCore : public CoreProcessor {
	using Info = NoiseInfo;
	using ThisCore = NoiseCore;

public:
	NoiseCore() = default;

	void update() override {
		whiteNoise = MathTools::randomNumber(-1.f * MaxOutputVolts, MaxOutputVolts);
		b0 = 0.99765 * b0 + whiteNoise * 0.0990460;
		b1 = 0.96300 * b1 + whiteNoise * 0.2965164;
		b2 = 0.57000 * b2 + whiteNoise * 1.0526913;
		pinkNoise = b0 + b1 + b2 + whiteNoise * 0.1848;
	}

	void set_param(int param_id, float val) override {
	}

	void set_input(int input_id, float val) override {
	}

	float get_output(int output_id) const override {
		if (output_id == Info::OutputWhite)
			return whiteNoise;

		if (output_id == Info::OutputPink)
			return pinkNoise;

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
	float whiteNoise = 0;
	float pinkNoise = 0;

	float b0 = 0.f;
	float b1 = 0.f;
	float b2 = 0.f;
};

} // namespace MetaModule
