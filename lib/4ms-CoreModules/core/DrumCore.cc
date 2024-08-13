#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Drum_info.hh"
#include "processors/envelope.h"
#include "processors/twoOpFMOscillator.h"

namespace MetaModule
{

class DrumCore : public CoreProcessor {
	using Info = DrumInfo;
	using ThisCore = DrumCore;

private:
	enum { pitchEnvelope, fmEnvelope, toneEnvelope, noiseEnvelope };

	Envelope envelopes[4];
	TwoOpFM osc;

	float gateIn = 0;
	float drumOutput = 0;
	float baseFrequency = 50;
	float noiseBlend = 0.5f;
	float pitchAmount = 0;
	float fmAmount = 0;
	float pitchCV = 20;
	bool pitchConnected = false;

	float baseNoiseEnvTime = 0;
	float baseToneEnvTime = 0;
	float basePitchEnvTime = 0;
	float baseFMEnvTime = 0;

	float noiseEnvCV = 0;
	float toneEnvCV = 0;
	float pitchEnvCV = 0;
	float FMEnvCV = 0;

	float ratio = 1;

	InterpArray<float, 4> pitchDecayTimes = {10, 10, 200, 500};
	InterpArray<float, 4> pitchBreakPoint = {0, 0.1, 0.2, 1};
	InterpArray<float, 4> pitchReleaseTimes = {50, 300, 500, 3000};

	InterpArray<float, 7> toneAttackTimes = {1, 1, 3, 5, 7, 9, 20};
	InterpArray<float, 6> toneHoldTimes = {0, 20, 50, 70, 100, 600};
	InterpArray<float, 3> toneDecayTimes = {10, 200, 600};
	InterpArray<float, 4> toneBreakPoint = {0.1, 0.2, 0.8};
	InterpArray<float, 3> toneReleaseTimes = {10, 500, 4000};

public:
	DrumCore() {
		for (int i = 0; i < 4; i++) {
			envelopes[i].sustainEnable = false;
			envelopes[i].set_attack_curve(0);
			envelopes[i].set_decay_curve(0);
			envelopes[i].set_release_curve(0.5f);
			envelopes[i].set_sustain(0.2f);
		}

		envelopes[toneEnvelope].set_envelope_time(0, 1);
		envelopes[toneEnvelope].set_envelope_time(1, 50);
		envelopes[toneEnvelope].set_envelope_time(2, 100);
		envelopes[toneEnvelope].set_envelope_time(3, 2000);
		envelopes[toneEnvelope].set_release_curve(1.0f);

		envelopes[fmEnvelope].set_envelope_time(0, 1);
		envelopes[fmEnvelope].set_envelope_time(1, 0);
		envelopes[fmEnvelope].set_envelope_time(2, 300);
		envelopes[fmEnvelope].set_envelope_time(3, 700);

		envelopes[noiseEnvelope].set_envelope_time(0, 1);
		envelopes[noiseEnvelope].set_envelope_time(1, 0);
		envelopes[noiseEnvelope].set_envelope_time(2, 30);
		envelopes[noiseEnvelope].set_envelope_time(3, 700);
		envelopes[noiseEnvelope].set_release_curve(0.0f);

		envelopes[pitchEnvelope].set_envelope_time(0, 5.0);
		envelopes[pitchEnvelope].set_envelope_time(1, 0);
		envelopes[pitchEnvelope].set_envelope_time(2, 50);
		envelopes[pitchEnvelope].set_envelope_time(3, 2000);

		setToneEnvelope();
		setFMEnvelope();
		setNoiseEnvelope();
		setPitchEnvelope();
	}

	void update() override {
		auto freqCalc =
			baseFrequency + (envelopes[pitchEnvelope].update(gateIn) * 4000.0f * (pitchAmount * pitchAmount));
		osc.set_frequency(1, baseFrequency * ratio);
		if (pitchConnected) {
			osc.set_frequency(0, freqCalc * MathTools::setPitchMultiple(pitchCV));
		} else {
			osc.set_frequency(0, freqCalc);
		}

		osc.modAmount = envelopes[fmEnvelope].update(gateIn) * fmAmount;
		auto noiseOut = MathTools::randomNumber(-1.0f, 1.0f) * envelopes[noiseEnvelope].update(gateIn);

		auto toneOutput = osc.update() * envelopes[toneEnvelope].update(gateIn);

		drumOutput = MathTools::interpolate(toneOutput, noiseOut, noiseBlend);
	}

	void set_param(int param_id, float val) override {
		switch (param_id) {
			case Info::KnobPitch:
				baseFrequency = MathTools::map_value(val, 0.0f, 1.0f, 10.0f, 1000.0f);
				break;
			case Info::KnobPitch_Env: // pitch envelope
				basePitchEnvTime = val;
				setPitchEnvelope();
				break;
			case Info::KnobPitch_Amt:
				pitchAmount = val;
				break;
			case Info::KnobRatio:
				ratio = MathTools::map_value(val, 0.0f, 1.0f, 1.0f, 16.0f);
				break;
			case Info::KnobFm_Env: // fm envelope
				baseFMEnvTime = val;
				setFMEnvelope();
				break;
			case Info::KnobFm_Amt:
				fmAmount = val;
				break;
			case Info::KnobTone_Env: // tone envelope
				baseToneEnvTime = val;
				setToneEnvelope();
				break;
			case Info::KnobNoise_Env: // noise envelope
				baseNoiseEnvTime = val;
				setNoiseEnvelope();
				break;
			case Info::KnobNoise_Blend:
				noiseBlend = val;
				break;
		}
	}

	void setFMEnvelope() {
		float val = MathTools::constrain(baseFMEnvTime + FMEnvCV, 0.0f, 1.0f);
		envelopes[fmEnvelope].set_envelope_time(0, MathTools::map_value(val, 0.0f, 1.0f, 1.0f, 100.0f));
		envelopes[fmEnvelope].set_envelope_time(2, MathTools::map_value(val, 0.0f, 1.0f, 10.0f, 8000.0f));
		envelopes[fmEnvelope].set_envelope_time(3, MathTools::map_value(val, 0.0f, 1.0f, 10.0f, 3000.0f));
		envelopes[fmEnvelope].set_sustain(MathTools::map_value(val, 0.0f, 1.0f, 0.0f, 0.3f));
	}

	void setToneEnvelope() {
		float val = MathTools::constrain(baseToneEnvTime + toneEnvCV, 0.0f, 1.0f);
		envelopes[toneEnvelope].set_envelope_time(0, toneAttackTimes.interp(val));
		envelopes[toneEnvelope].set_envelope_time(1, toneHoldTimes.interp(val));
		envelopes[toneEnvelope].set_envelope_time(2, toneDecayTimes.interp(val));
		envelopes[toneEnvelope].set_envelope_time(3, toneReleaseTimes.interp(val));
		envelopes[toneEnvelope].set_sustain(toneBreakPoint.interp(val));
	}

	void setNoiseEnvelope() {
		float val = MathTools::constrain(baseNoiseEnvTime + noiseEnvCV, 0.0f, 1.0f);
		envelopes[noiseEnvelope].set_envelope_time(0, MathTools::map_value(val, 0.0f, 1.0f, 1.0f, 50.0f));
		envelopes[noiseEnvelope].set_envelope_time(2, MathTools::map_value(val, 0.0f, 1.0f, 30.0f, 100.0f));
		envelopes[noiseEnvelope].set_envelope_time(3, MathTools::map_value(val, 0.0f, 1.0f, 100.0f, 3000.0f));
		envelopes[noiseEnvelope].set_sustain(MathTools::map_value(val, 0.0f, 1.0f, 0.0f, 0.25f));
	}

	void setPitchEnvelope() {
		float val = MathTools::constrain(pitchEnvCV + basePitchEnvTime, 0.0f, 1.0f);
		envelopes[pitchEnvelope].set_envelope_time(2, pitchDecayTimes.interp(val));
		envelopes[pitchEnvelope].set_envelope_time(3, pitchReleaseTimes.interp(val));
		envelopes[pitchEnvelope].set_sustain(pitchBreakPoint.interp(val));
	}

	void set_samplerate(float sr) override {
		for (int i = 0; i < 4; i++) {
			envelopes[i].set_samplerate(sr);
		}
		osc.set_samplerate(sr);
	}

	void set_input(int input_id, float val) override {
		val = val / CvRangeVolts;

		switch (input_id) {
			case Info::InputTrigger:
				gateIn = val;
				break;
			case Info::InputV_Oct:
				pitchCV = val;
				break;
			case Info::InputN_Env_Cv:
				noiseEnvCV = val;
				setNoiseEnvelope();
				break;
			case Info::InputFm_Env_Cv:
				FMEnvCV = val;
				setFMEnvelope();
				break;
			case Info::InputP_Env_Cv:
				pitchEnvCV = val;
				setPitchEnvelope();
				break;
			case Info::InputT_Env_Cv:
				toneEnvCV = val;
				setToneEnvelope();
				break;

			case Info::InputP_Amt_Cv:
				break;
			case Info::InputN_Blend_Cv:
				break;
			case Info::InputFm_Amt_Cv:
				break;
			case Info::InputRatio_Cv:
				break;
		}
	}

	float get_output(int output_id) const override {
		if (output_id == Info::OutputOut)
			return drumOutput * outputVolts;
		return 0.f;
	}

	void mark_input_unpatched(int input_id) override {
		if (input_id == Info::InputV_Oct) {
			pitchConnected = false;
		}
	}
	void mark_input_patched(int input_id) override {
		if (input_id == Info::InputV_Oct) {
			pitchConnected = true;
		}
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

	static constexpr float outputVolts = 5.f;
};

} // namespace MetaModule
