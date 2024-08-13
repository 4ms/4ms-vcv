#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/EnOsc_info.hh"

#include "enosc/easiglib/dsp.hh"
using namespace easiglib;
#include "enosc/ui.hh"

namespace MetaModule
{

class EnOscCore : public CoreProcessor {
	using Info = EnOscInfo;
	using ThisCore = EnOscCore;

	enum { kUiUpdateRate = 60 };
	enum { kUiProcessRate = 20 };

public:
	EnOscCore() = default;

	void update() override {
		// Low-priority thread
		// in while loop:
		if (ui_process_ctr++ > ui_process_throttle) {
			ui_process_ctr = 0;
			enosc.Process(); //EventHandler::Process
		}

		if (ui_update_ctr++ > ui_update_throttle) {
			ui_update_ctr = 0;
			enosc.Update(); //LED update
		}

		// SampleRate / BlockRate (6kHz for 48k)
		if (++block_ctr >= EnOsc::kBlockSize) {
			block_ctr = 0;
			enosc.Poll();
			enosc.osc().Process(out_block_);
		}
	}

	// DOWN=0 / MID=0.5 / UP=1.0
	EnOsc::Switches::State switchstate(float val) {
		using enum EnOsc::Switches::State;
		return val < 0.25f ? DOWN : val < 0.75f ? MID : UP;
	}

	void set_param(int param_id, float val) override {
		using AdcInput = EnOsc::AdcInput;

		if (param_id < Info::NumKnobs) {
			switch (param_id) {
				case Info::KnobBalance:
					enosc.set_potcv(AdcInput::POT_BALANCE, val);
					break;
				case Info::KnobCross_Fm:
					enosc.set_potcv(AdcInput::POT_MOD, val);
					break;
				case Info::KnobDetune:
					enosc.set_potcv(AdcInput::POT_DETUNE, val);
					break;
				case Info::KnobPitch:
					enosc.set_potcv(AdcInput::POT_PITCH, val);
					break;
				case Info::KnobRoot:
					enosc.set_potcv(AdcInput::POT_ROOT, val);
					break;
				case Info::KnobScale:
					enosc.set_potcv(AdcInput::POT_SCALE, val);
					break;
				case Info::KnobSpread:
					enosc.set_potcv(AdcInput::POT_SPREAD, val);
					break;
				case Info::KnobTwist:
					enosc.set_potcv(AdcInput::POT_TWIST, val);
					break;
				case Info::KnobWarp:
					enosc.set_potcv(AdcInput::POT_WARP, val);
					break;
			}

		} else if (param_id < ((int)Info::NumKnobs + (int)Info::NumSwitches)) {
			switch (param_id - Info::NumKnobs) {
				case Info::SwitchScale_Switch:
					enosc.switches().scale_.set(switchstate(val));
					break;
				case Info::SwitchCross_Fm_Switch:
					enosc.switches().mod_.set(switchstate(val));
					break;
				case Info::SwitchTwist_Switch:
					enosc.switches().twist_.set(switchstate(val));
					break;
				case Info::SwitchWarp_Switch:
					enosc.switches().warp_.set(switchstate(val));
					break;
				case Info::SwitchLearn:
					enosc.set_learn_button(val > 0.5f);
					break;
				case Info::SwitchFreeze:
					enosc.set_freeze_button(val > 0.5f);
					break;
			}

		} else {
			switch (param_id - (int)Info::NumKnobs - (int)Info::NumSwitches) {
				case Info::AltParamStereosplit: {
					auto mode = static_cast<EnOsc::SplitMode>(val * 2.9f);
					enosc.set_stereo_mode(mode);
				} break;
				case Info::AltParamNumosc: {
					int num_osc = (val * 15.9f) + 1;
					enosc.set_num_osc(num_osc);
				} break;
				case Info::AltParamCrossfade:
					enosc.set_crossfade(val);
					break;
				case Info::AltParamFreezesplit: {
					auto mode = static_cast<EnOsc::SplitMode>(val * 2.9f);
					enosc.set_freeze_mode(mode);
				} break;
				case Info::AltParamFinetune:
					enosc.set_fine_tune(val);
					break;
			}
		}
	}

	void set_input(int input_id, float val) override {
		using AdcInput = EnOsc::AdcInput;
		using SpiAdcInput = EnOsc::SpiAdcInput;

		val /= 5.f;	  //-5V to +5V => -1..1
		val *= -0.5f; //-1..1 => 0.5..-0.5
		val += 0.5f;  // => 1..0
					  // Ui::set_potcv will clamp
		switch (input_id) {
			case Info::InputBalance_Jack:
				enosc.set_potcv(AdcInput::CV_BALANCE, val);
				break;
			case Info::InputCross_Fm_Jack:
				enosc.set_potcv(AdcInput::CV_MOD, val);
				break;
			case Info::InputPitch_Jack:
				enosc.set_pitchroot_cv(SpiAdcInput::CV_PITCH, val);
				break;
			case Info::InputRoot_Jack:
				enosc.set_pitchroot_cv(SpiAdcInput::CV_ROOT, val);
				break;
			case Info::InputScale_Jack:
				enosc.set_potcv(AdcInput::CV_SCALE, val);
				break;
			case Info::InputSpread_Jack:
				enosc.set_potcv(AdcInput::CV_SPREAD, val);
				break;
			case Info::InputTwist_Jack:
				enosc.set_potcv(AdcInput::CV_TWIST, val);
				break;
			case Info::InputWarp_Jack:
				enosc.set_potcv(AdcInput::CV_WARP, val);
				break;
			case Info::InputFreeze_Jack:
				enosc.set_freeze_gate(val > 0.5f);
				break;
			case Info::InputLearn_Jack:
				enosc.set_learn_gate(val > 0.5f);
				break;
		}
	}

	float get_output(int output_id) const override {
		s9_23 sample = output_id == 0 ? out_block_[block_ctr].l : out_block_[block_ctr].r;

		//hardware EnOssc is about 4.5Vpp for one osc, we make it 2x as loud to match other virtual VCOs
		auto s = f::inclusive(sample).repr() * 9.f;
		return s;
	}

	void set_samplerate(float sr) override {
		if (sample_rate_ != sr) {
			sample_rate_ = sr;
			ui_process_throttle = (unsigned)sample_rate_ / kUiProcessRate;
			ui_update_throttle = (unsigned)sample_rate_ / kUiUpdateRate;

			enosc.set_samplerate(sr);
		}
	}

	float get_led_brightness(int led_id) const override {
		if (led_id < 0 || led_id > 5)
			return 0.f;

		if (led_id < 3) {
			auto c = enosc.get_learn_led_color();
			auto el = led_id == 0 ? c.red() : led_id == 1 ? c.green() : c.blue();
			return f::inclusive(el).repr();
		} else {
			auto c = enosc.get_freeze_led_color();
			auto el = led_id == 3 ? c.red() : led_id == 4 ? c.green() : c.blue();
			return f::inclusive(el).repr();
		}
	}

	void mark_all_inputs_unpatched() override {
		for (unsigned i = 0; i < Info::NumInJacks; i++)
			set_input(i, 0.f);
	}

	void mark_input_unpatched(const int input_id) override {
		set_input(input_id, 0.f);
	}

	void mark_input_patched(const int input_id) override {
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	EnOsc::Ui<kUiUpdateRate, EnOsc::kBlockSize> enosc;
	Buffer<EnOsc::Frame, EnOsc::kBlockSize> out_block_;
	EnOsc::DynamicData dydata;

	float sample_rate_ = 48000.f;

	unsigned ui_process_throttle = (unsigned)sample_rate_ / kUiProcessRate;
	unsigned ui_process_ctr = ui_process_throttle;

	unsigned ui_update_throttle = (unsigned)sample_rate_ / kUiUpdateRate;
	unsigned ui_update_ctr = ui_update_throttle;

	unsigned block_ctr = EnOsc::kBlockSize;
};

} // namespace MetaModule
