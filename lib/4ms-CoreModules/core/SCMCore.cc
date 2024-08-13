#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "SCMPlus/cv_skip.h"
#include "info/SCM_info.hh"
#include "util/countzip.hh"
#include "util/math.hh"
#include "util/zip.hh"

#include <cstdio>

namespace MetaModule
{

class SCMCore : public SmartCoreProcessor<SCMInfo> {
	using Info = SCMInfo;
	using ThisCore = SCMCore;
	using enum Info::Elem;

public:
	SCMCore() = default;

	uint32_t convert_param(float rawval, float max) {
		return MathTools::constrain<float>(rawval * max, 0.f, 255.f);
	}

	void update() override {
		uint32_t rotate_adc = convert_param(getState<RotateKnob>() + getInput<RotateJackIn>().value_or(0) / 5.f, 7.99f);
		uint32_t slippage_adc = convert_param(getState<SlipKnob>() + getInput<SlipJackIn>().value_or(0) / 5.f, 255);
		uint32_t shuffle_adc = convert_param(getState<ShuffleKnob>() + getInput<ShuffleJackIn>().value_or(0) / 5.f, 255);
		uint32_t skip_adc = convert_param(getState<SkipKnob>() + getInput<SkipJackIn>().value_or(0) / 5.f, 255);
		uint32_t pw_adc = convert_param(getState<PwKnob>() + getInput<PwJackIn>().value_or(0) / 5.f, 255);

		bool faster_switch_state = getState<_4XFastButton>() == LatchingButton::State_t::DOWN;
		if (getInput<_4XFastJackIn>().value_or(0) > 2.5f)
			faster_switch_state = !faster_switch_state;

		mute = getState<MuteButton>() == LatchingButton::State_t::DOWN;
		if (getInput<MuteJackIn>().value_or(0) > 2.5f)
			mute = !mute;

		setLED<_4XFastButton>(faster_switch_state);
		setLED<MuteButton>(mute);

		auto clockInValue = getInput<ClkIn>().value_or(0);

		setLED<LedInLight>(clockInValue);

		if (clockInValue > 0.5f) {
			if (!clkin) {
				clkin = true;
				handle_clock_in();
			}
		} else
			clkin = false;

		// We "tick" every update
		// To make it more like the hardware SCM, we would tick less often
		// But that'd introduce more jitter and less accurate clock
		handle_tick();

		if ((rotate_adc != old_rotation) || (faster_switch_state != old_faster_switch_state)) {
			old_rotation = rotate_adc;
			old_faster_switch_state = faster_switch_state;

			for (auto [i, _d, _dd] : enumerate(d, dd)) {
				_dd = ((i + rotate_adc) & 7) + 1;
				_d = faster_switch_state ? _dd << 2 : _dd;
			}
			d[6] = d[7];
			dd[6] = dd[7];

			update_pulse_params = true;
		}

		if ((slippage_adc != old_slippage_adc) || (pw_adc != old_pw_adc)) {
			old_pw_adc = pw_adc;
			old_slippage_adc = slippage_adc;
			update_pulse_params = true;
		}

		skip_pattern = (skip_adc > 128) ? ~skip[0xFF - skip_adc] : skip[skip_adc];

		if (shuffle_adc != old_shuffle_adc) {
			old_shuffle_adc = shuffle_adc;
			uint16_t t = shuffle_adc & 0x7F; // chop to 0-127|0-127
			t = (t * 5) >> 7;				 // scale 0..127 to 0-4
			slip_every = t + 2;				 // shift to 2-6

			if (shuffle_adc <= 127)
				slip_howmany = 0;
			else
				slip_howmany = ((shuffle_adc & 0b00011000) >> 3) + 1; // 1..4

			if (slip_howmany >= slip_every)
				slip_howmany = slip_every - 1; // 1..(slip_every-1)
		}

		auto resyncInValue = getInput<ResyncIn>().value_or(0);

		if (resyncInValue > 0.5f && !resync) {
			resync = true;
			for (auto [_p, _s, _slip, _slipamt] : zip(p, s, slip, slipamt)) {
				_p = 0;
				_s = 0;
				_slip = _slipamt;
			}
		} else if (!resyncInValue)
			resync = false;

		if (DoFreeRun) {
			if (tmr_int > period) {
				tmr_int -= period;

				reset_all_phases();

				update_pulse_params = true;
				update_slip_params = true;
			}
		} else {
			if (tmr_clkin >= (period << 1))
				is_running = false;
		}

		if (update_pulse_params) {
			auto base_pw = calc_pw(pw_adc, period);
			for (auto [_pw, _d] : zip(pw, d))
				_pw = base_pw / _d;

			// uint32_t min_pw = MIN_PW + 1;
			for (auto [_pw, _d, _per] : zip(pw, d, per)) {
				if (_per >= MIN_PW && _pw < MIN_PW)
					_pw = MIN_PW;
				_per = period / _d;
			}

			if (slippage_adc < 2) {
				for (auto &_slipamt : slipamt)
					_slipamt = 0;
			} else {
				for (auto [_pw, _per, _slipamt] : zip(pw, per, slipamt)) {
					_slipamt = static_cast<int32_t>((slippage_adc * (_per - (_pw + MIN_PW))) >> 8);
				}
			}

			if (update_slip_params) { // clock in received
				for (auto [_slipamt, _slip] : zip(slipamt, slip))
					_slip = _slipamt;
			}

			update_pulse_params = false;
			update_slip_params = false;
		}

		setOutput<X1Out>(outs[Info::OutputX1] ? 8.0f : 0.0f);
		setOutput<X2Out>(outs[Info::OutputX2] ? 8.0f : 0.0f);
		setOutput<S3Out>(outs[Info::OutputS3] ? 8.0f : 0.0f);
		setOutput<S4Out>(outs[Info::OutputS4] ? 8.0f : 0.0f);
		setOutput<S5Out>(outs[Info::OutputS5] ? 8.0f : 0.0f);
		setOutput<S6Out>(outs[Info::OutputS6] ? 8.0f : 0.0f);
		setOutput<S8Out>(outs[Info::OutputS8] ? 8.0f : 0.0f);
		setOutput<X8Out>(outs[Info::OutputX8] ? 8.0f : 0.0f);

		setLED<LedX1Light>(outs[Info::OutputX1] ? 1.0f : 0);
		setLED<LedX2Light>(outs[Info::OutputX2] ? 1.0f : 0);
		setLED<LedS3Light>(outs[Info::OutputS3] ? 1.0f : 0);
		setLED<LedS4Light>(outs[Info::OutputS4] ? 1.0f : 0);
		setLED<LedS5Light>(outs[Info::OutputS5] ? 1.0f : 0);
		setLED<LedS6Light>(outs[Info::OutputS6] ? 1.0f : 0);
		setLED<LedS8Light>(outs[Info::OutputS8] ? 1.0f : 0);
		setLED<LedX8Light>(outs[Info::OutputX8] ? 1.0f : 0);
	}

	void reset_all_phases() {
		if (!mute) {
			for (auto &_o : outs)
				_o = true;
		}

		for (auto &_t : tmr)
			_t = 0;

		for (auto &_p : p)
			_p = 0;

		for (auto &_s : s)
			_s = 0;
	}

	void handle_clock_in() {
		period = tmr_clkin;
		is_running = true;

		tmr_clkin = 0;
		tmr_int = 0;

		reset_all_phases();

		update_pulse_params = true;
		update_slip_params = true;
	}

	void handle_tick() {
		tmr_clkin++;
		tmr_int++;
		for (auto [_tmr, _pw, _out] : zip(tmr, pw, outs)) {
			_tmr++;
			if (_tmr >= _pw)
				_out = false;
		}

		if (is_running) {
			constexpr std::array<uint32_t, 3> plain_jacks{0, 1, 7};
			for (const auto i : plain_jacks) {
				if (tmr[i] > per[i]) {
					tmr[i] = tmr[i] - per[i];
					if (!mute)
						outs[i] = true;
				}
			}

			constexpr std::array complex_jacks{2, 3, 4, 5, 6};
			for (const auto i : complex_jacks) {
				if (tmr[i] > (per[i] + slip[i])) {
					tmr[i] = 0;

					if ((skip_pattern >> (8 - dd[i])) & (1 << p[i]))
						if (!mute)
							outs[i] = true;

					if (++p[i] >= dd[i]) {
						p[i] = 0;
						s[i] = 0;
						slip[i] = slipamt[i];
					}

					if (s[i] == slip_howmany)
						slip[i] = -1 * slipamt[i];
					else
						slip[i] = 0;

					if (++s[i] >= slip_every) {
						s[i] = 0;
						slip[i] = slipamt[i];
					}
				}
			}
		}
	}

	uint32_t calc_pw(uint8_t pw_adc, uint32_t period) {
		float pw = (float)pw_adc / 255.f * (float)period;
		return std::clamp<uint32_t>(pw, MIN_PW, period - MIN_PW);
	}

	void set_samplerate(float sr) override {
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	// Controls/Outs
	std::array<bool, Info::NumOutJacks> outs;

	static constexpr bool DoFreeRun = true; //TODO: make this a switch?

	// Params (analog)
	uint32_t old_shuffle_adc = 127;
	uint32_t old_slippage_adc = 127;
	uint32_t old_pw_adc = 127;
	uint32_t old_rotation = 255;
	uint32_t old_faster_switch_state = 127;

	// Params (rhythmic)
	uint8_t skip_pattern = 0xFF;
	uint8_t slip_every = 2;
	uint8_t slip_howmany = 0;

	// Params (states)
	bool clkin;
	bool mute;
	bool is_running;
	bool resync;

	// Flags
	bool update_pulse_params;
	bool update_slip_params;

	// Multiply-by for each jack
	std::array<uint32_t, 8> d;

	// Phase counters
	uint32_t tmr_int = 0;
	uint32_t tmr_clkin = 0;

	std::array<uint32_t, 8> tmr{};
	uint32_t period = 84000;
	std::array<uint32_t, 8> per;

	// Pulse-width
	std::array<uint32_t, 8> pw{8400, 4200, 2800, 2200, 1680, 1400, 1050, 1050};
	static constexpr uint32_t MIN_PW = 50;

	// Beat counters for Skip feature
	std::array<uint32_t, 8> p;

	// Counters for slip
	std::array<uint32_t, 8> s;
	std::array<uint32_t, 8> dd;
	std::array<int32_t, 8> slipamt;
	std::array<int32_t, 8> slip;
};

} // namespace MetaModule
