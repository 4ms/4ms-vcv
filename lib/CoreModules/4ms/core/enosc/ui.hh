#pragma once

#include "adaptors/buttons.hh"
#include "adaptors/color.hh"
#include "adaptors/switches.hh"
#include "control.hh"
#include "easiglib/bitfield.hh"
#include "easiglib/event_handler.hh"
#include "polyptic_oscillator.hh"

namespace EnOsc
{

template<int update_rate>
struct LedManager {
	using Color = EnOsc::Color;
	using Colors = EnOsc::Colors;

	LedManager(Color::Adjustment &color_cal, Color initial)
		: color_cal_(color_cal)
		, display_(initial) {
	}

	// flash_freq in Hz; max = update_rate
	void flash(Color c, f flash_freq = 10_f) {
		flash_color_ = c;
		flash_phase_ = max_val<u0_16>;
		flash_freq_ = u0_16(flash_freq / f(update_rate));
	}

	void set_background(Color c) {
		background_color_ = c;
	}
	void set_solid(Color c) {
		solid_color_ = c;
	}

	// freq in secs
	void set_glow(Color c, f freq = 1_f) {
		glow_color_ = c;
		osc_.set_frequency(u0_32(freq / f(update_rate)));
	}

	void reset_glow(u0_32 phase = 0._u0_32) {
		osc_.set_frequency(0._u0_32);
		osc_.set_phase(phase);
	}

	void Update() {
		Color c = background_color_;
		if (solid_color_ != Colors::black)
			c = solid_color_;
		c = c.blend(glow_color_, u0_8::narrow(osc_.Process()));
		c = c.blend(flash_color_, u0_8::narrow(flash_phase_));
		c = c.adjust(color_cal_);

		//T::set(c);
		display_ = c;

		if (flash_phase_ > flash_freq_)
			flash_phase_ -= flash_freq_;
		else
			flash_phase_ = 0._u0_16;
	}

	void set_cal(f r, f g, f b) {
		color_cal_.r = u1_7(r);
		color_cal_.g = u1_7(g);
		color_cal_.b = u1_7(b);
	}

	Color get_color() const {
		return display_;
	}

private:
	easiglib::TriangleOscillator osc_;
	Color background_color_ = Colors::black;
	Color solid_color_ = Colors::black;
	Color flash_color_ = Colors::white;
	Color glow_color_ = Colors::red;
	u0_16 flash_freq_ = 0.0014_u0_16;
	u0_16 flash_phase_ = 0._u0_16;
	Color::Adjustment &color_cal_;

	Color display_;
};

struct ButtonsEventSource : EventSource<Event>, Buttons {
	void Poll(std::function<void(Event)> const &put) override {
		Buttons::Debounce();
		if (Buttons::learn_.just_pushed())
			put({ButtonPush, BUTTON_LEARN});
		else if (Buttons::learn_.just_released())
			put({ButtonRelease, BUTTON_LEARN});
		if (Buttons::freeze_.just_pushed())
			put({ButtonPush, BUTTON_FREEZE});
		else if (Buttons::freeze_.just_released())
			put({ButtonRelease, BUTTON_FREEZE});
	}
};

template<class Switch, EventType event>
struct SwitchEventSource : EventSource<Event>, Switch {
	void Poll(std::function<void(Event)> const &put) final {
		Switch::Debounce();
		if (Switch::just_switched_up())
			put({event, Switches::UP});
		else if (Switch::just_switched_mid())
			put({event, Switches::MID});
		else if (Switch::just_switched_down())
			put({event, Switches::DOWN});
	}
};

struct SwitchesEventSource : EventSource<Event>, Switches {

	SwitchEventSource<Scale, SwitchScale> scale_;
	SwitchEventSource<Mod, SwitchMod> mod_;
	SwitchEventSource<Twist, SwitchTwist> twist_;
	SwitchEventSource<Warp, SwitchWarp> warp_;

	void Poll(std::function<void(Event)> const &put) final {
		scale_.Poll(put);
		mod_.Poll(put);
		twist_.Poll(put);
		warp_.Poll(put);
	}
};

template<int update_rate, int block_size>
class Ui : public EventHandler<Ui<update_rate, block_size>, Event> {
	using Base = EventHandler<Ui, Event>;
	using Color = EnOsc::Color;
	using Colors = EnOsc::Colors;
	friend Base;

	Parameters params_;
	PolypticOscillator<block_size> osc_{params_};

	Persistent<WearLevel<FlashBlock<1, Parameters::AltParameters>>> alt_params_{&params_.alt, params_.default_alt};

	static constexpr int kProcessRate = kSampleRate / block_size;
	static constexpr int kLongPressTime = 4.0f * kProcessRate;	   // sec
	static constexpr int kNewNoteDelayTime = 0.01f * kProcessRate; // sec

	f cached_pitch_base_;

	struct LedCalibrationData {
		Color::Adjustment led_learn_adjust;
		Color::Adjustment led_freeze_adjust;

		bool validate() {
			return true;
		}
	};
	LedCalibrationData led_calibration_data_;
	LedCalibrationData default_led_calibration_data_ = {{1._u1_7, 1._u1_7, 1._u1_7}, {1._u1_7, 1._u1_7, 1._u1_7}};

	Persistent<WearLevel<FlashBlock<3, LedCalibrationData>>> led_calibration_data_storage_{
		&led_calibration_data_, default_led_calibration_data_};

	LedManager<update_rate> learn_led_{led_calibration_data_.led_learn_adjust, Colors::yellow};
	LedManager<update_rate> freeze_led_{led_calibration_data_.led_freeze_adjust, Colors::yellow};

	typename Base::DelayedEventSource button_timeouts_[2];
	typename Base::DelayedEventSource new_note_delay_;
	ButtonsEventSource buttons_;
	SwitchesEventSource switches_;
	Control<block_size> control_{params_, osc_};

	EventSource<Event> *sources_[6] = {
		&buttons_, &switches_, &button_timeouts_[0], &button_timeouts_[1], &control_, &new_note_delay_};

	enum Mode {
		NORMAL,
		SHIFT,
		LEARN,
		MANUAL_LEARN,
	} mode_ = NORMAL;

	Bitfield<32> active_catchups_{0};

	void reset_leds() {
		learn_led_.set_background(Colors::lemon);
		freeze_led_.set_background(Colors::lemon);
		learn_led_.reset_glow();
		freeze_led_.reset_glow();
	}

	void Handle(typename Base::EventStack stack) {
		Event &e1 = stack.get(0);
		Event &e2 = stack.get(1);

		switch (e1.type) {
			case ScaleChange: {
				if (mode_ == NORMAL || mode_ == LEARN || mode_ == MANUAL_LEARN)
					learn_led_.flash(Colors::white);
			} break;
			case ButtonPush: {
				button_timeouts_[e1.data].trigger_after(kLongPressTime, {ButtonTimeout, e1.data});
			} break;
			case SwitchScale: {
				params_.scale.mode = e1.data == Switches::UP ? TWELVE : e1.data == Switches::MID ? OCTAVE : FREE;
			} break;
			case SwitchMod: {
				params_.modulation.mode = e1.data == Switches::UP ? ONE : e1.data == Switches::MID ? TWO : THREE;
			} break;
			case SwitchTwist: {
				params_.twist.mode = e1.data == Switches::UP ? FEEDBACK : e1.data == Switches::MID ? PULSAR : CRUSH;
			} break;
			case SwitchWarp: {
				params_.warp.mode = e1.data == Switches::UP ? FOLD : e1.data == Switches::MID ? CHEBY : SEGMENT;
			} break;
			default:
				break;
		}

		switch (mode_) {

			case NORMAL: {

				switch (e1.type) {
					case ButtonRelease: {
						if (e2.type == ButtonPush && e1.data == e2.data) {
							// Learn pressed
							if (e1.data == BUTTON_LEARN) {
								mode_ = LEARN;
								learn_led_.set_solid(Colors::dark_red);
								cached_pitch_base_ = osc_.lowest_pitch() - control_.pitch_cv();
								osc_.enable_learn();
								control_.hold_pitch_cv();
							}
						}
					} break;
					case ButtonTimeout: {
						if (e1.data == BUTTON_LEARN && e2.type == ButtonPush && e2.data == BUTTON_LEARN) {
							// long-press on Learn
							osc_.reset_current_scale();
							control_.all_main_function();
							learn_led_.flash(Colors::blue, 2_f);
						}
					} break;
					case ButtonPush: {
						if (e1.data == BUTTON_FREEZE) {
							mode_ = SHIFT;
							//stop updating pot values when the Shift button
							//goes down, in case the user performs an alt function
							control_.disable_all_alt_shift_pot_values();
						}
						if (e1.data == BUTTON_LEARN)
							control_.disable_all_alt_learn_pot_values();
					} break;
					default:
						break;
				}
			} break;

			case SHIFT: {
				switch (e1.type) {
					case ButtonRelease: {
						if (e1.data == BUTTON_FREEZE) {
							osc_.set_freeze(!osc_.frozen());
							freeze_led_.set_solid(osc_.frozen() ? Colors::blue : Colors::black);
							mode_ = NORMAL;
							control_.all_main_function();
						}
					} break;
					case AltParamChange: {
						freeze_led_.flash(Colors::white);
					} break;
					default:
						break;
				}
			} break;

			case LEARN: {
				switch (e1.type) {
					case NewNoteAfterDelay: {
						new_note_delay_.trigger_after(kNewNoteDelayTime, {NewNote, 0});
					} break;
					case NewNote: {
						bool success = osc_.new_note(control_.pitch_cv() + cached_pitch_base_);
						osc_.enable_pre_listen();
						learn_led_.flash(success ? Colors::white : Colors::black);
					} break;
					case ButtonRelease: {
						if (e1.data == BUTTON_LEARN && e2.type == ButtonPush && e2.data == BUTTON_LEARN) {
							// Learn pressed
							mode_ = NORMAL;
							bool success = osc_.disable_learn();
							if (success)
								learn_led_.flash(Colors::green, 2_f);
							control_.release_pitch_cv();
							control_.all_main_function();
							learn_led_.reset_glow();
							learn_led_.set_solid(Colors::black);
						} else if (e1.data == BUTTON_FREEZE && e2.type == ButtonPush && e2.data == BUTTON_FREEZE) {
							// Freeze pressed
							bool success = osc_.remove_last_note();
							if (success)
								freeze_led_.flash(Colors::black);
						}
					} break;
					default:
						break;
				}
				default:
					break;
			} break;

			case MANUAL_LEARN: {
				if (e1.type == ButtonRelease && e1.data == BUTTON_LEARN) {
					osc_.disable_follow_new_note();
					mode_ = LEARN;
				}
			} break;
		}
	}

public:
	Ui() {
		// Initialize switches to their current positions
		Base::put({SwitchScale, switches_.scale_.get()});
		Base::put({SwitchMod, switches_.mod_.get()});
		Base::put({SwitchTwist, switches_.twist_.get()});
		Base::put({SwitchWarp, switches_.warp_.get()});
		Base::Process();

		mode_ = NORMAL;
		learn_led_.set_background(Colors::lemon);
		freeze_led_.set_background(Colors::lemon);
	}

	PolypticOscillator<block_size> &osc() {
		return osc_;
	}

	void Poll() {
		control_.ProcessSpiAdcInput();
		Base::Poll();
		freeze_led_.set_solid(osc_.frozen() ? Colors::blue : Colors::black);
	}

	void Update() {
		learn_led_.Update();
		freeze_led_.Update();
	}

	// Hardware model getter/setters:

	void set_potcv(AdcInput chan, float val) {
		control_.set_potcv(chan, val);
	}
	void set_pitchroot_cv(SpiAdcInput chan, float val) {
		control_.set_pitchroot_cv(chan, val);
	}
	auto &switches() {
		return switches_;
	}
	void set_learn_button(bool val) {
		buttons_.learn_.set(val);
	}
	void set_freeze_button(bool val) {
		buttons_.freeze_.set(val);
	}
	void set_freeze_gate(bool val) {
		control_.set_gate(GATE_FREEZE, val);
	}
	void set_learn_gate(bool val) {
		control_.set_gate(GATE_LEARN, val);
	}
	Color get_freeze_led_color() const {
		return freeze_led_.get_color();
	}
	Color get_learn_led_color() const {
		return learn_led_.get_color();
	}
	void set_fine_tune(float val) {
		control_.set_fine_tune(val);
	}
	void set_crossfade(float val) {
		control_.set_crossfade(val);
	}
	void set_num_osc(int val) {
		control_.set_num_osc(val);
	}
	void set_freeze_mode(SplitMode val) {
		control_.set_freeze_mode(val);
	}
	void set_stereo_mode(SplitMode val) {
		control_.set_stereo_mode(val);
	}

	void set_samplerate(float new_sample_rate) {
		params_.sample_rate = f(new_sample_rate);
	}
};

} // namespace EnOsc
