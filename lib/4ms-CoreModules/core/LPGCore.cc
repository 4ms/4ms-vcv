#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/LPG_info.hh"

#include "lpg/envelope.h"
#include "lpg/low_pass_gate.h"
#include "lpg/units.h"

#include "helpers/EdgeDetector.h"
#include "helpers/FlipFlop.h"

namespace MetaModule
{

class LPGCore : public SmartCoreProcessor<LPGInfo> {
	using Info = LPGInfo;
	using ThisCore = LPGCore;
	using enum Info::Elem;

public:
	LPGCore()
		: sampleRate(48000.f), pingIn(0.5f, 2.f) {
		lpg.Init();
		envelope.Init();
	};

	void update() override {
		float in = getInput<InputIn>().value_or(0.f);

		bool pingPatched = false;

		if(isPatched<PingIn>()) {
			pingPatched = true;
			if(pingEdge(pingIn(getInput<PingIn>().value()))){

				envelope.Trigger();
			}
		}

		auto add_cv_and_pot = [](std::optional<float> cv, float pot) {
			const float cv_val = cv.value_or(0.f) / 5.f; // range: -1 .. 1 for CV -5V .. +5V
			return std::clamp(pot + cv_val, 0.f, 1.f);
		};

		const auto lpg_colour = add_cv_and_pot(getInput<ColorCvIn>(), getState<ColorKnob>());
		const auto decay = add_cv_and_pot(getInput<DecayCvIn>(), getState<DecayKnob>());
		const auto level = add_cv_and_pot(getInput<LevelCvIn>(), getState<LevelKnob>());

		const float short_decay = (200.0f * 1.f) / sampleRate * LPG::stmlib::SemitonesToRatio(-96.0f * decay);
    	const float decay_tail = (20.0f * 1.f) / sampleRate * LPG::stmlib::SemitonesToRatio(-72.0f * decay + 12.0f * lpg_colour) - short_decay;
		const float attack = (440.0f / 8.f) / sampleRate * 0.25f * LPG::stmlib::SemitonesToRatio(0.f) * float(1) * 2.0f;

		

		if(pingPatched) {
			envelope.ProcessPing(attack, short_decay, decay_tail, lpg_colour);
		} else {
			envelope.ProcessLP(level, short_decay, decay_tail, lpg_colour);
		}
		

		lpg.Process(envelope.gain(), envelope.frequency(), envelope.hf_bleed(), &in, 1);

		setOutput<Out>(in);
	}

	void set_samplerate(float sr) override {
		sampleRate = sr;
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	float sampleRate;

private:
	plaits::LowPassGate lpg;
	plaits::LPGEnvelope envelope;

private:
	FlipFlop pingIn;
	EdgeDetector pingEdge;

private:
};

} // namespace MetaModule
