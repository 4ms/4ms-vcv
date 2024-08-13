#pragma once
#include "audio_stream_conf.hh"
#include "calibration_storage.hh"
#include "clock_mult_util.hh"
#include "controls.hh"
#include "epp_lut.hh"
#include "flags.hh"
#include "log_taper_lut.hh"
#include "modes.hh"
#include "system_mode.hh"
#include "timer.hh"
#include "util/countzip.hh"
#include "util/math.hh"

#include "../mocks/conf.hh"

namespace LDKit
{
// Params holds all the modes, settings and parameters for the looping delay
// Params are set by controls (knobs, jacks, buttons, etc)
struct Params {
	Controls &controls;
	Flags &flags;

	// TODO: double-buffer Params:
	// put just these into its own struct
	float time = 1.f;		 // TIME: fractional value for time multiplication, integer value for time division
	float delay_feed = 0.7f; // DELAY FEED: amount of main input mixed into delay loop
	float feedback = 0.5f;	 // FEEDBACK: amount of regeneration
	float mix_dry = 0.7f;	 // MIX: mix of delayed and clean on the main output
	float mix_wet = 0.7f;
	float tracking_comp = 1.f;	// TRACKING_COMP: -2.0 .. 2.0 compensation for 1V/oct tracking
	float divmult_time = 12000; // samples between read and write heads
	uint32_t ping_time = 12000;
	uint32_t locked_ping_time = 12000;

	uint32_t currentSampleRate = 48000;

	ChannelMode modes;
	OperationMode op_mode = OperationMode::Normal;

	PersistentStorage persistent_storage;
	Settings &settings = persistent_storage.data.settings;
	Timer timer{settings.ping_method};

	SystemMode sys_mode;

	Params(Controls &controls, Flags &flags)
		: controls{controls}
		, flags{flags}
		, sys_mode{controls, flags, persistent_storage} {
	}

	void start() {
		timer.start();
	}

	void update() {
		controls.update();

		update_trig_jacks();

		update_pot_states();
		update_cv_states();

		update_adjust_loop_end_mode();
		update_time_quant_mode();

		calc_delay_feed();
		calc_feedback();
		calc_time();
		calc_mix();

		if (modes.inf == InfState::On)
			update_scroll_loop_amount();

		// TODO
		tracking_comp = 1.f;

		if (op_mode == OperationMode::Calibrate) {
			// TODO: Calibrate mode
			//  update_calibration();
		}

		if (op_mode == OperationMode::SysSettings) {
			sys_mode.update();

			if (sys_mode.is_done()) {
				ignore_inf_release = false;
				ignore_rev_release = false;
				op_mode = OperationMode::Normal;
			}
		}

		if (op_mode == OperationMode::Normal) {
			update_leds();
			update_button_modes();
		}

		if (flags.mute_on_boot_ctr)
			flags.mute_on_boot_ctr--;
	}

	void reset_loop() {
		timer.reset_loop_tmr();
	}

	// TODO: to use a double-buffer params, then
	// looping delay should set a flag that tells params to set a
	// new state for these
	void set_inf_state(InfState newstate) {
		modes.inf = newstate;
	}
	void toggle_reverse() {
		modes.reverse = !modes.reverse;
	}
	void set_divmult(float new_divmult) {
		divmult_time = new_divmult;
		timer.set_divmult_time(divmult_time);
	}

private:
	void update_trig_jacks() {
		if (controls.rev_jack.is_just_pressed() ||
			(settings.rev_jack == GateType::Gate && controls.rev_jack.is_just_released()))
		{
			if (modes.quantize_mode_changes)
				flags.set_rev_quantized_changed();
			else
				flags.set_rev_changed();
		}
		if (controls.inf_jack.is_just_pressed() ||
			(settings.inf_jack == GateType::Gate && controls.inf_jack.is_just_released()))
		{
			if (modes.quantize_mode_changes)
				flags.set_inf_quantized_changed();
			else
				flags.set_inf_changed();
		}

		if (timer.take_ping_changed()) {
			controls.ping_led.high();
			ping_time = timer.get_ping_time();
			if (!modes.ping_locked)
				flags.set_time_changed();
			handle_quantized_mode_changes();
		}

		if (timer.take_ping_cycled()) {
			handle_quantized_mode_changes();
		}
	}

	void update_pot_states() {
		for (auto [i, pot] : enumerate(pot_state)) {
			pot.cur_val = (int16_t)controls.read_pot(static_cast<PotAdcElement>(i++));

			int16_t diff = pot.cur_val - pot.prev_val;
			if (std::abs(diff) > Brain::MinPotChange)
				pot.track_moving_ctr = 10; // track for at least 6ms

			if (pot.track_moving_ctr) {
				pot.track_moving_ctr--;
				pot.prev_val = pot.cur_val;
				pot.delta = diff;
				pot.moved = true;

				if (controls.rev_button.is_pressed()) {
					pot.moved_while_rev_down = true;
					ignore_rev_release = true; // if i==TimePot and in InfMode only?
				}

				if (controls.inf_button.is_pressed()) {
					pot.moved_while_inf_down = true;
					ignore_inf_release = true; // if i==TimePot || FeedbackPot in InfMode only?
				}
			} else {
				pot.moved = false;
				pot.delta = 0;
			}
		}
	}

	void update_cv_states() {
		for (auto [i, cv] : enumerate(cv_state)) {
			cv.cur_val = (int16_t)controls.read_cv(static_cast<CVAdcElement>(i++));
			if (op_mode == OperationMode::Calibrate) {
				// TODO: use raw values, without calibration offset
			}
			int16_t diff = cv.cur_val - cv.prev_val;
			if (std::abs(diff) > Brain::MinCVChange) {
				cv.delta = diff;
				cv.prev_val = cv.cur_val;
			}
		}
	}

	void update_button_modes() {
		// Press ping (and no other buttons)
		if (!controls.rev_button.is_pressed() && !controls.inf_button.is_pressed()) {
			if (controls.ping_button.is_just_pressed()) {
				ping_time = timer.reset_ping_tmr();
				handle_quantized_mode_changes();

				if (!modes.ping_locked) {
					flags.set_time_changed();
				}
			}
		}

		if (controls.inf_button.is_just_released()) {
			if (!ignore_inf_release) {
				if (modes.quantize_mode_changes)
					flags.set_inf_quantized_changed();
				else
					flags.set_inf_changed();
			}

			ignore_inf_release = false;
			for (auto &pot : pot_state)
				pot.moved_while_inf_down = false;
		}

		if (controls.rev_button.is_just_released()) {
			if (!ignore_rev_release) {
				if (modes.quantize_mode_changes)
					flags.set_rev_quantized_changed();
				else
					flags.set_rev_changed();
			}

			ignore_rev_release = false;
			for (auto &pot : pot_state)
				pot.moved_while_rev_down = false;
		}

		// Stereo mode
		if (!ignore_inf_release && controls.inf_button.how_long_held_pressed() > 1500) {
			if (!ignore_rev_release && controls.rev_button.how_long_held_pressed() > 1500) {
				if (!controls.ping_button.is_pressed()) {
					settings.stereo_mode = !settings.stereo_mode;
					flag_animate_stereo = settings.stereo_mode ? 1500 : 0;
					flag_animate_mono = settings.stereo_mode ? 0 : 1500;
					flags.set_time_changed();
					ignore_inf_release = true;
					ignore_rev_release = true;
				}
			}
		}

		// System Mode
		if (!ignore_inf_release && controls.inf_button.how_long_held_pressed() > 3000) {
			if (!ignore_rev_release && controls.rev_button.how_long_held_pressed() > 3000) {
				if (controls.ping_button.how_long_held_pressed() > 3000) {
					ignore_inf_release = true;
					ignore_rev_release = true;

					if (controls.read_time_switch() == Controls::SwitchPos::Up) {
						sys_mode.reset();
						op_mode = OperationMode::SysSettings;
					} else {
						flags.set_clear_memory();
						flag_animate_clear = 1000;
					}
				}
			}
		}

		// quantized change mode:
		switch (qcm_state) {
			case QcmState::Idle:
				if (controls.rev_button.is_just_pressed() && !controls.inf_button.is_pressed() &&
					!controls.ping_button.is_pressed())
					qcm_state = QcmState::RevPressed;
				break;
			case QcmState::RevPressed:
				if (controls.rev_button.is_pressed() && !controls.inf_button.is_pressed() &&
					controls.ping_button.is_just_pressed())
					qcm_state = QcmState::RevPingPressed;
				break;
			case QcmState::RevPingPressed:
				if (controls.rev_button.is_pressed() && !controls.inf_button.is_pressed() &&
					controls.ping_button.is_just_released())
				{
					qcm_state = QcmState::Idle;
					modes.quantize_mode_changes = !modes.quantize_mode_changes;
					flag_acknowledge_qcm = 1536;
					ignore_rev_release = true;
				}
				break;
		}

		// Reset to idle if all buttons released
		if (!controls.rev_button.is_pressed() && !controls.inf_button.is_pressed() &&
			!controls.ping_button.is_pressed())
		{
			qcm_state = QcmState::Idle;
		}
	}

	void update_leds() {
		if (controls.ping_button.is_pressed()) {
			controls.ping_led.high();
		}

		if (timer.ping_led_high())
			controls.ping_led.high();
		else
			controls.ping_led.low();

		if (flag_animate_stereo) {
			flag_animate_stereo--;
			if ((flag_animate_stereo & 0xFF) == 0x80) {
				controls.rev_led.low();
				controls.inf_led.high();
			}
			if ((flag_animate_stereo & 0xFF) == 0x00) {
				controls.rev_led.high();
				controls.inf_led.low();
			}
		} else if (flag_animate_mono) {
			flag_animate_mono--;
			if ((flag_animate_mono & 0x1FF) == 0x100) {
				controls.rev_led.high();
				controls.inf_led.high();
			}
			if ((flag_animate_mono & 0x1FF) == 0x000) {
				controls.rev_led.low();
				controls.inf_led.low();
			}
		} else if (flag_animate_clear) {
			flag_animate_clear--;
			controls.rev_led.high();
			controls.inf_led.high();

		} else if (flag_acknowledge_qcm) {
			flag_acknowledge_qcm--;
			bool blink = (flag_acknowledge_qcm & (1 << 8)) ||
						 (!modes.quantize_mode_changes && (flag_acknowledge_qcm & (1 << 6)));
			controls.rev_led.set(blink);
			controls.ping_led.set(blink);
		}

		else
		{
			controls.rev_led.set(modes.reverse);
			if (modes.inf == InfState::TransitioningOn || modes.inf == InfState::On)
				controls.inf_led.high();
			else if (modes.inf == InfState::TransitioningOff || modes.inf == InfState::Off)
				controls.inf_led.low();
		}
	}

	void handle_quantized_mode_changes() {
		// Handle staged inf/rev changes due to quantize_mode_changes==true
		if (flags.take_time_quantized_changed())
			flags.set_time_changed();
		if (flags.take_inf_quantized_changed())
			flags.set_inf_changed();
		if (flags.take_rev_quantized_changed())
			flags.set_rev_changed();
	}

	void update_time_quant_mode() {
		// Holding INF while turning Time changes to NoQuant mode
		// Turning Time without INF down changes it back to Quant mode (default)
		if (pot_state[TimePot].moved_while_inf_down) {
			modes.time_pot_quantized = false;
			modes.time_cv_quantized = false;
		} else if (pot_state[TimePot].moved) {
			modes.time_pot_quantized = true;
			if (!settings.auto_unquantize_timejack)
				modes.time_cv_quantized = true;
		}

		if (settings.auto_unquantize_timejack) {
			// Auto Unquantize mode: set jack to Quant when time is > 23Hz
			modes.time_cv_quantized = (divmult_time >= currentSampleRate / 23);
		}
	}

	void update_adjust_loop_end_mode() {
		// In INF mode, holding REV and turning Time adjusts the loop end
		if (pot_state[TimePot].moved_while_rev_down && modes.inf == InfState::On)
			modes.adjust_loop_end = true;
		else
			modes.adjust_loop_end = false;
	}

	void update_scroll_loop_amount() {
		if (pot_state[FeedbackPot].moved_while_inf_down) {
			window_mode = true;
		} else if (pot_state[FeedbackPot].moved) {
			window_mode = false;
		}
		if (window_mode) {
			float scroll_amt = (pot_state[FeedbackPot].delta + cv_state[FeedbackCV].delta) / 4096.f;
			scroll_amt = std::clamp(scroll_amt, -1.f, 1.f);
			flags.add_scroll_amt(scroll_amt);
			pot_state[FeedbackPot].moved_while_inf_down = false;
			pot_state[FeedbackPot].delta = 0;
			cv_state[FeedbackCV].delta = 0;
		}
	}

	void calc_delay_feed() {
		if (modes.inf == InfState::On) {
			delay_feed = 0.f;
			return;
		}

		uint16_t df = __USAT(pot_state[DelayFeedPot].prev_val + cv_state[DelayFeedCV].prev_val, 12);

		if (settings.log_delay_feed)
			delay_feed = log_taper[df];
		else
			delay_feed = df > 4065 ? 1.f : (float)MathTools::plateau<30, 0>(df) / 4035.f;
	}

	void calc_feedback() {
		if (modes.inf == InfState::On) {
			feedback = 1.f;
			return;
		}

		float fb_pot = pot_state[FeedbackPot].prev_val;
		float fb;
		if (fb_pot < 3500.f)
			fb = fb_pot / 3500.f;
		else if (fb_pot < 4000.f)
			fb = 1.f;
		else
			fb = (fb_pot - 3050.f) / 950.f; //(4095-3050)/950 = 110% ... (4000-3050)/950 = 100%

		float fb_cv = cv_state[FeedbackCV].prev_val;
		// FIXME: DLD firmware has bug that prevents the fb += 1.f branch
		if (fb_cv > 4080.f)
			fb += 1.f;
		else if (fb_cv > 30.f)
			fb += fb_cv / 4095.f;

		if (fb > 1.1f)
			fb = 1.1f;
		else if (fb > 0.997f && fb < 1.003f)
			fb = 1.0f;

		feedback = fb;
	}

	void calc_time() {
		int16_t time_pot = pot_state[TimePot].cur_val;
		int16_t time_cv = MathTools::plateau<60, 0>(2048 - cv_state[TimeCV].cur_val);

		float time_pot_mult =
			modes.time_pot_quantized ? ClockMultUtil::calc_quantized(time_pot) : ClockMultUtil::calc_unquant(time_pot);

		float time_cv_mult = modes.time_cv_quantized ? ClockMultUtil::calc_quantized(time_cv) :
							 (time_cv < 0)			 ? ClockMultUtil::calc_unquant(time_cv) :
													   ClockMultUtil::calc_voct(time_cv, tracking_comp);

		auto time_switch = controls.read_time_switch();
		float time_mult = adjust_time_by_switch(time_pot_mult * time_cv_mult, time_switch);

		if (time != time_mult) {
			time = time_mult;
			if (modes.quantize_mode_changes)
				flags.set_time_quantized_changed();
			else
				flags.set_time_changed();
		}
	}

	void calc_mix() {
		uint16_t mx = __USAT(cv_state[MixCV].prev_val + pot_state[MixPot].prev_val, 12);

		mix_dry = epp_lut[mx];
		mix_wet = epp_lut[4095 - mx];
	}

	// void update_button_modes() {
	// }

	static constexpr float adjust_time_by_switch(float timeval, Controls::SwitchPos switch_pos) {
		if (switch_pos == Controls::SwitchPos::Up)
			return timeval + 16.f; // switch up: 17-32
		if (switch_pos == Controls::SwitchPos::Down)
			return timeval / 8.f; // switch down: eighth notes
		return timeval;
	}

private:
	struct PotState {
		int16_t cur_val = 0;
		int16_t prev_val = 0;			   // old_i_smoothed_potadc
		int16_t track_moving_ctr = 0;	   // track_moving_pot
		int16_t delta = 0;				   // pot_delta
		bool moved_while_inf_down = false; // flag_pot_changed_infdown
		bool moved_while_rev_down = false; // flag_pot_changed_revdown
		bool moved = false;				   // flag_pot_changed
	};
	std::array<PotState, NumPots> pot_state;

	struct CVState {
		int16_t cur_val = 0;
		int16_t prev_val = 0;
		int16_t delta = 0;
	};
	std::array<CVState, NumPots> cv_state;

	bool ignore_inf_release = false;
	bool ignore_rev_release = false;

	uint32_t flag_animate_mono = 0;
	uint32_t flag_animate_stereo = 0;
	uint32_t flag_acknowledge_qcm = 0;
	uint32_t flag_animate_clear = 0;

	bool window_mode = false;

	enum class QcmState { Idle, RevPressed, RevPingPressed } qcm_state;
};

constexpr auto ParamsSize = sizeof(Params);

} // namespace LDKit
