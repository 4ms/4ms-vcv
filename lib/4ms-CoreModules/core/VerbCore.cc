#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/Verb_info.hh"
#include "processors/allpass.h"
#include "processors/comb.h"
#include "util/math.hh"

namespace MetaModule
{

class VerbCore : public CoreProcessor {
	using Info = VerbInfo;
	using ThisCore = VerbCore;

public:
	VerbCore() {
		globalAllpassAtten = 1.0f;
		globalCombAtten = 1.0f;
		for (int i = 0; i < numAllpass; i++) {
			manualKnobAllpassAtten[i] = 1.0f;
			ratioKnobAllpassAtten[i] = 1.0f;
			apFilter[i].setLength(maxAllpassTuning[i]);
			apFilter[i].setFeedback(0.6f);
			apFilter[i].setFadeSpeed(0.001f);
		}

		for (int i = 0; i < numComb; i++) {
			manualKnobCombAtten[i] = 1.0f;
			ratioKnobCombAtten[i] = 1.0f;
			combFilter[i].setFeedback(0);
			combFilter[i].setLength(maxCombTuning[i]);
			combFilter[i].setFadeSpeed(0.001f);
		}
	}

	void update() override {
		update_params();

		float wetSignal = 0;

		for (int i = 0; i < numComb; i++) {
			wetSignal += combFilter[i].process(signalIn);
		}

		wetSignal /= static_cast<float>(numComb);

		for (int i = 0; i < numAllpass; i++) {
			wetSignal = apFilter[i].process(wetSignal);
		}

		signalOut = MathTools::interpolate(signalIn, wetSignal, mix);
		// signalOut = signalIn * inv_mix + wetSignal * mix;
	}

	void set_param(int param_id, float val) override {
		switch (param_id) {
			case VerbInfo::KnobSize:
				globalAllpassAtten = val;
				globalCombAtten = val;
				all_comb_tuning_needs_update = true;
				all_ap_tuning_needs_update = true;
				break;

			case VerbInfo::KnobDamp:
				new_damp = val;
				damp_needs_update = true;
				break;

			case VerbInfo::KnobMix:
				mix = val;
				inv_mix = 1.0f - val;
				break;

			case VerbInfo::KnobTime:
				new_feedback = val;
				fb_needs_update = true;
				break;

			case VerbInfo::KnobAp_Ratio:
				new_all_ap_ratio = val;
				all_ap_ratio_needs_update = true;
				break;

			case VerbInfo::KnobComb:
				new_all_comb_ratio = val;
				all_comb_ratio_needs_update = true;
				break;

				// case Allpass1:
				// case Allpass2:
				// case Allpass3:
				// case Allpass4: {
				// 	int i = param_id - Allpass1;
				// 	ap_ratio_needs_update[i] = true;
				// 	new_ap_ratio[i] = val;
				// } break;

				// case C1:
				// case C2:
				// case C3:
				// case C4:
				// case C5:
				// case C6:
				// case C7:
				// case C8: {
				// 	int i = param_id - C1;
				// 	comb_ratio_needs_update[i] = true;
				// 	new_comb_ratio[i] = val;
				// } break;
		}
	}

	float getAllpassTuning(const int id) {
		return currentAllpassTuning[id];
	}
	float getCombTuning(const int id) {
		return currentCombTuning[id];
	}

	void updateAllpassTuning(const int id) {
		currentAllpassTuning[id] =
			maxAllpassTuning[id] * globalAllpassAtten * manualKnobAllpassAtten[id] * ratioKnobAllpassAtten[id];
		if (currentAllpassTuning[id] < 1.f)
			currentAllpassTuning[id] = 1.f;
		apFilter[id].setLength(currentAllpassTuning[id]);
	}

	void updateCombTuning(const int id) {
		currentCombTuning[id] = maxCombTuning[id] * globalCombAtten * manualKnobCombAtten[id] * ratioKnobCombAtten[id];
		if (currentCombTuning[id] < 1.f)
			currentCombTuning[id] = 1.f;
		combFilter[id].setLength(currentCombTuning[id]);
	}

	void update_params() {
		if (all_ap_tuning_needs_update) {
			for (int i = 0; i < numAllpass; i++)
				updateAllpassTuning(i);
			all_ap_tuning_needs_update = false;
		}
		if (all_comb_tuning_needs_update) {
			for (int i = 0; i < numComb; i++)
				updateCombTuning(i);
			all_comb_tuning_needs_update = false;
		}
		if (damp_needs_update) {
			for (int i = 0; i < numComb; i++) {
				combFilter[i].setDamp(new_damp);
			}
			damp_needs_update = false;
		}
		if (fb_needs_update) {
			auto v = MathTools::map_value(new_feedback, 0.0f, 1.0f, 0.8f, 0.99f);
			for (int i = 0; i < numComb; i++) {
				combFilter[i].setFeedback(v);
			}
		}
		if (all_ap_ratio_needs_update) {
			int ival = (int)(new_all_ap_ratio * 48); // 0...48
			float fval = ival / 12.f + 1.f;			 // 1..5, steps of 0.08333
			for (int i = 0; i < numAllpass; i++) {
				ratioKnobAllpassAtten[i] = i == 0 ? 1.f : ratioKnobAllpassAtten[i - 1] / fval;
				updateAllpassTuning(i);
			}
		}
		if (all_comb_ratio_needs_update) {
			int ival = (int)(new_all_comb_ratio * 48);
			float fval = ival / 12.f + 1.f;
			for (int i = 0; i < numComb; i++) {
				ratioKnobCombAtten[i] = i == 0 ? 1.f : ratioKnobCombAtten[i - 1] / fval;
				updateCombTuning(i);
			}
		}
		for (int i = 0; i < numAllpass; i++) {
			if (ap_ratio_needs_update[i]) {
				manualKnobCombAtten[i] = new_ap_ratio[i];
				updateAllpassTuning(i);
				ap_ratio_needs_update[i] = false;
			}
		}
		for (int i = 0; i < numComb; i++) {
			if (comb_ratio_needs_update[i]) {
				manualKnobCombAtten[i] = new_comb_ratio[i];
				updateCombTuning(i);
				comb_ratio_needs_update[i] = false;
			}
		}
	}
	void set_input(int input_id, float val) override {
		if (input_id == 0) //FIXME: Input jacks
			signalIn = val;
	}

	float get_output(int output_id) const override {
		if (output_id == Info::OutputOut)
			return signalOut;
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
	float signalIn = 0;
	float signalOut = 0;

	static constexpr int numAllpass = 4;
	static constexpr int numComb = 8;

	// static constexpr int defaultAllpassTuning[numAllpass] = {1248, 812, 358, 125};
	// static constexpr int defaultCombTuning[numComb] = {3000, 4003, 4528, 5217, 1206, 2108, 3337, 5003};
	static constexpr int maxAllpassTuning[numAllpass] = {6000, 6000, 6000, 6000};
	static constexpr int maxCombTuning[numComb] = {6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000};

	float globalAllpassAtten;
	float globalCombAtten;
	float manualKnobAllpassAtten[numAllpass];
	float manualKnobCombAtten[numComb];

	float ratioKnobAllpassAtten[numAllpass];
	float ratioKnobCombAtten[numComb];
	// Todo: Add CV
	// float ratioCvAllpassAtten[numAllpass];
	// float ratioCvCombAtten[numAllpass];

	float currentAllpassTuning[numAllpass];
	float currentCombTuning[numComb];

	AllPass<6000> apFilter[numAllpass];
	Comb<6000> combFilter[numComb];

	float mix = 0.f;
	float inv_mix = 1.f;

	bool all_ap_tuning_needs_update = true;
	bool all_comb_tuning_needs_update = true;
	float new_damp = 0.f;
	bool damp_needs_update = true;
	float new_feedback = 0.f;
	bool fb_needs_update = true;
	float new_all_ap_ratio = 0.f;
	bool all_ap_ratio_needs_update = true;
	float new_all_comb_ratio = 0.f;
	bool all_comb_ratio_needs_update = true;

	float new_ap_ratio[numAllpass] = {0.f};
	bool ap_ratio_needs_update[numAllpass] = {false};
	float new_comb_ratio[numComb] = {0.f};
	bool comb_ratio_needs_update[numComb] = {false};
};

} // namespace MetaModule
