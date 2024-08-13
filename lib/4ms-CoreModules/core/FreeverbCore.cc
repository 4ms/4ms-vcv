#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Freeverb_info.hh"
#include "processors/allpass.h"
#include "processors/comb.h"
#include "processors/tools/dcBlock.h"
#include "util/math.hh"
#include "util/zip.hh"

namespace MetaModule
{

class FreeverbCore : public SmartCoreProcessor<FreeverbInfo> {
	using Info = FreeverbInfo;
	using ThisCore = FreeverbCore;
	using enum Info::Elem;

public:
	FreeverbCore() {
		for (auto [comb, def, curCombTuning] : zip(combFilter, DefaultCombTuning, currentCombTuning)) {
			comb.setFeedback(0);
			comb.setLength(def);
		}

		for (auto [ap, def, curApTuning] : zip(apFilter, DefaultAllPassTuning, currentAllPassTuning)) {
			ap.setLength(def);
			ap.setFeedback(0.6f);
			ap.setFadeSpeed(0.001f);
		}
	}

	void update() override {

		auto add_cv_and_pot = [](std::optional<float> cv, float pot) {
			const float cv_val = cv.value_or(0.f) / 5.f; // range: -1 .. 1 for CV -5V .. +5V
			return std::clamp(pot + cv_val, 0.f, 1.f);
		};

		if (auto size = add_cv_and_pot(getInput<SizeCvIn>(), getState<SizeKnob>()); prev_size != size) {
			prev_size = size;
			setSize(size);
		}

		if (auto damp = add_cv_and_pot(getInput<DampCvIn>(), getState<DampKnob>()); prev_damp != damp) {
			prev_damp = damp;
			setDamp(damp);
		}

		if (auto fb = add_cv_and_pot(getInput<FeedbackCvIn>(), getState<FeedbackKnob>()); prev_fb != fb) {
			prev_fb = fb;
			setFeedback(fb);
		}

		float dry = getInput<InputIn>().value_or(0.f);
		float wet = 0;
		for (auto &comb : combFilter) {
			wet += comb.process(dry);
		}

		wet /= static_cast<float>(NumComb);

		for (auto &allpass : apFilter) {
			wet = allpass.process(wet);
		}

		const auto mix = add_cv_and_pot(getInput<MixCvIn>(), getState<MixKnob>());

		setOutput<Out>(dcblock.update(MathTools::interpolate3(dry, wet, mix)));
	}

	void setSize(float val) {
		val *= (MaxSize - MinSize);
		val += MinSize;
		val *= sr_ratio;

		for (auto [comb, def, curCombTuning] : zip(combFilter, DefaultCombTuning, currentCombTuning)) {
			curCombTuning = std::clamp(def * val, 100.f, (float)MaxCombSize);
			comb.setLength(curCombTuning);
		}

		for (auto [ap, def, curApTuning] : zip(apFilter, DefaultAllPassTuning, currentAllPassTuning)) {
			curApTuning = std::clamp(def * val, 40.f, (float)MaxAPSize);
			ap.setLength(curApTuning);
		}
	}

	void setDamp(float val) {
		for (auto &comb : combFilter) {
			comb.setDamp(val);
		}
	}

	void setFeedback(float val) {
		for (auto &comb : combFilter) {
			comb.setFeedback(MathTools::map_value(val, 0.0f, 1.0f, 0.8f, 1.0f));
		}
	}

	void set_samplerate(float sr) override {
		sr_ratio = sr / 48000.f;
		prev_size = -1.f; //force re-calculate
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	static const int NumComb = 8;
	static const int NumAllPass = 4;

	static constexpr std::array<int, NumComb> DefaultCombTuning{1215, 1293, 1390, 1476, 1548, 1623, 1695, 1760};
	static constexpr std::array<int, NumAllPass> DefaultAllPassTuning{605, 480, 371, 245};

	static constexpr float MaxSize = 2.5f;
	static constexpr float MinSize = 0.23f;
	static constexpr size_t MaxCombSize = MaxSize * 1760 /* max(DefaultCombTuning) */;
	static constexpr size_t MaxAPSize = MaxSize * 605 /* max(DefaultAllPassTuning) */;

	std::array<int, NumAllPass> currentAllPassTuning{DefaultAllPassTuning};
	std::array<int, NumComb> currentCombTuning{DefaultCombTuning};

	std::array<Comb<MaxCombSize>, NumComb> combFilter{};
	std::array<AllPass<MaxAPSize>, NumAllPass> apFilter{};

	float prev_size{-1};
	float prev_damp{-1};
	float prev_fb{-1};

	float sr_ratio = 1.f;

	DCBlock dcblock;
};

} // namespace MetaModule
