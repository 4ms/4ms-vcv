#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/ComplexEG_info.hh"
#include "processors/envelope.h"
#include "util/math.hh"
using namespace MathTools;

namespace MetaModule
{

class ComplexEGCore : public SmartCoreProcessor<ComplexEGInfo> {
	using Info = ComplexEGInfo;
	using ThisCore = ComplexEGCore;
	using enum Info::Elem;

public:
	ComplexEGCore() = default;

	void update() override {
		isLooping = getState<LoopButton>() == LatchingButton::State_t::DOWN ? true : false;

		float finalAttack =
			constrain(getInput<AttackCvIn>().value_or(0.f) / CvRangeVolts + getState<AttackKnob>(), 0.0f, 1.0f);
		float finalHold =
			constrain(getInput<HoldCvIn>().value_or(0.f) / CvRangeVolts + getState<HoldKnob>(), 0.0f, 1.0f);
		float finalDecay =
			constrain(getInput<DecayCvIn>().value_or(0.f) / CvRangeVolts + getState<DecayKnob>(), 0.0f, 1.0f);
		float finalSustain =
			constrain(getInput<SustainCvIn>().value_or(0.f) / CvRangeVolts + getState<SustainKnob>(), 0.0f, 1.0f);
		float finalRelease =
			constrain(getInput<ReleaseCvIn>().value_or(0.f) / CvRangeVolts + getState<ReleaseKnob>(), 0.0f, 1.0f);

		e.set_envelope_time(e.ATTACK, map_value(finalAttack, 0.0f, 1.0f, 0.1f, 1000.0f));
		e.set_envelope_time(e.HOLD, map_value(finalHold, 0.0f, 1.0f, 0.0f, 1000.0f));
		e.set_envelope_time(e.DECAY, map_value(finalDecay, 0.0f, 1.0f, 0.1f, 1000.0f));
		e.set_envelope_time(e.RELEASE, map_value(finalRelease, 0.0f, 1.0f, 0.1f, 1000.0f));

		e.set_sustain(finalSustain);

		e.set_attack_curve(getState<ACurveKnob>());
		e.set_decay_curve(getState<DCurveKnob>());
		e.set_release_curve(getState<RCurveKnob>());

		if (isLooping) {
			if (currentStage == Envelope::IDLE) {
				envelopeOutput = e.update(8.f);
			} else {
				envelopeOutput = e.update(0.f);
			}
		} else {
			envelopeOutput = e.update(getInput<GateIn>().value_or(0.f));
		}

		currentStage = e.getStage();

		setOutput<AttackOut>((currentStage == e.ATTACK) ? MaxOutputVolts : 0);
		setOutput<HoldOut>((currentStage == e.HOLD) ? MaxOutputVolts : 0);
		setOutput<DecayOut>((currentStage == e.DECAY) ? MaxOutputVolts : 0);
		setOutput<SustainOut>((currentStage == e.SUSTAIN) ? MaxOutputVolts : 0);
		setOutput<ReleaseOut>((currentStage == e.RELEASE) ? MaxOutputVolts : 0);

		setOutput<Out>(envelopeOutput * MaxOutputVolts);

		setLED<LoopButton>(isLooping ? 1.f : 0.f);
	}

	void set_samplerate(float sr) override {
		e.set_samplerate(sr);
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	bool isLooping = false;
	float envelopeOutput = 0;

	Envelope::stage_t currentStage = Envelope::stage_t::ATTACK;
	Envelope e;
};

} // namespace MetaModule
