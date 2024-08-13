#pragma once

#include "adaptors/adc.hh"
#include "adaptors/gates.hh"
#include "adaptors/persistent_storage.hh"
#include "adaptors/spi_adc.hh"
#include "easiglib/dsp.hh"
#include "easiglib/event_handler.hh"
#include "polyptic_oscillator.hh"

namespace EnOsc {

const f kPotDeadZone = 0.01_f;
const f kPitchPotRange = 6_f * 12_f;
const f kRootPotRange = 9_f * 12_f;
const f kNewNoteRange = 6_f * 12_f;
const f kFineTuneRange = 4_f;
const f kSpreadRange = 12_f;
const f kPotMoveThreshold = 0.01_f;

template<int CHAN, class FILTER>
class ExtCVConditioner {
	f &offset_;
	f nominal_offset_;
	f &slope_;
	f nominal_slope_;
	FILTER lp_;
	SpiAdc &spi_adc_;

	f last_raw_reading_;

public:
	ExtCVConditioner(f &o, f &s, SpiAdc &spi_adc)
		: offset_(o)
		, nominal_offset_(o)
		, slope_(s)
		, nominal_slope_(s)
		, spi_adc_(spi_adc) {
	}

	void Process() {
		u0_16 x = spi_adc_.get(CHAN);
		lp_.Process(x);
	}

	void switch_channel() {
		spi_adc_.switch_channel();
	}

	f last() {
		last_raw_reading_ = f::inclusive(lp_.last());
		return (last_raw_reading_ - offset_) * slope_;
	}
};

template<AdcInput INPUT>
class CVConditioner {
	Adc &adc_;
	f &offset_;

public:
	CVConditioner(Adc &adc, f &offset)
		: adc_(adc)
		, offset_(offset) {
	}

	f Process() {
		u0_16 in = adc_.get(INPUT);
		s1_15 x = in.to_signed_scale();
		return f::inclusive(x) - offset_;
	}
};

struct NoCVInput {
	NoCVInput(Adc &adc) {
	}
	f Process() {
		return 0._f;
	}
};

class MovementDetector {
	f previous_value_;

public:
	bool Process(f x) {
		f diff = (x - previous_value_).abs();
		if (diff > kPotMoveThreshold) {
			previous_value_ = x;
			return true;
		} else
			return false;
	}
};

enum class Law { LINEAR, QUADRATIC, CUBIC, QUARTIC };

template<AdcInput INPUT, Law LAW, class FILTER>
class PotConditioner : MovementDetector {
	Adc &adc_;
	FILTER filter_;

public:
	PotConditioner(Adc &adc)
		: adc_(adc) {
	}

	f raw() {
		return f::inclusive(adc_.get(INPUT));
	}

	f Process(std::function<void(Event)> const &put) {
		f x = f::inclusive(adc_.get(INPUT));
		x = Signal::crop(kPotDeadZone, x);
		switch (LAW) {
			case Law::LINEAR:
				break;
			case Law::QUADRATIC:
				x = x * x;
				break;
			case Law::CUBIC:
				x = x * x * x;
				break;
			case Law::QUARTIC:
				x = x * x;
				x = x * x;
				break;
		}
		x = filter_.Process(x);
		if (MovementDetector::Process(x))
			put({PotMove, INPUT});
		return x; // 0..1
	}
};

enum class Takeover { HARD, SOFT };

enum class PotFct { MAIN, ALT };

template<AdcInput INPUT, Law LAW, class FILTER, Takeover TO>
class DualFunctionPotConditioner : public PotConditioner<INPUT, LAW, FILTER> {
	enum State { MAIN, INACTIVE, ALT, ARMING, CATCHUP } state_ = MAIN;
	f main_value_;
	f alt_value_ = -1_f; // -1 indicates no value
public:
	DualFunctionPotConditioner(Adc &adc)
		: PotConditioner<INPUT, LAW, FILTER>(adc) {
	}
	DualFunctionPotConditioner(Adc &adc, SavedDualPotState saved_state)
		: PotConditioner<INPUT, LAW, FILTER>(adc) {
		if (saved_state.validate()) {
			main_value_ = saved_state.restore_main_val;
			alt_value_ = saved_state.restore_alt_val;
			if (saved_state.restore_catchup_mode == SavedDualPotState::CatchUpMode)
				state_ = ARMING;
		}
	}

	void alt() {
		state_ = ALT;
	}
	void main() {
		if (state_ == ALT)
			state_ = ARMING;
		else if (state_ == INACTIVE)
			state_ = MAIN;
	}
	void reset_alt_value() {
		alt_value_ = -1_f;
	}
	void disable() {
		if (state_ == MAIN)
			state_ = INACTIVE;
	}

	SavedDualPotState get_state() {
		return {.restore_catchup_mode = (state_ == MAIN) ? SavedDualPotState::MainMode : SavedDualPotState::CatchUpMode,
				.restore_main_val = main_value_,
				.restore_alt_val = alt_value_};
	}

	std::pair<f, f> Process(std::function<void(Event)> const &put) {
		f input = PotConditioner<INPUT, LAW, FILTER>::Process(put);
		switch (state_) {
			case MAIN: {
				main_value_ = input;
			} break;
			case INACTIVE: {
			} break;
			case ALT: {
				alt_value_ = input;
			} break;
			case ARMING: {
				state_ = CATCHUP;
				put({StartCatchup, INPUT});
			} break;
			case CATCHUP: {
				if (TO == Takeover::HARD) {
					if ((input - alt_value_).abs() > kPotMoveThreshold) {
						state_ = MAIN;
						put({EndOfCatchup, INPUT});
					}
				} else if (TO == Takeover::SOFT) {
					//Todo: set state_ to PLATEAUED_MAIN where the cached value is used
					//Then on movement detect set it to MAIN
					if ((input - main_value_).abs() <= 0.0005_f) {
						state_ = MAIN;
						put({EndOfCatchup, INPUT});
					}
				}
			} break;
		}
		return std::pair(main_value_, alt_value_);
	}
};

template<class PotConditioner, class CVConditioner, class FILTER>
class PotCVCombiner {
	FILTER filter_;

public:
	PotConditioner pot_;
	CVConditioner cv_;

	PotCVCombiner(Adc &adc)
		: pot_(adc)
		, cv_(adc) {
	}
	PotCVCombiner(Adc &adc, f &cv_offset)
		: pot_(adc)
		, cv_(adc, cv_offset) {
	}

	// TODO disable this function if PotConditioner = DualFunction
	f Process(std::function<void(Event)> const &put) {
		f x = pot_.Process(put);
		x -= cv_.Process();
		x = x.clip(0_f, 1_f);
		return filter_.Process(x);
	}

	std::pair<f, f> ProcessDualFunction(std::function<void(Event)> const &put) {
		auto [main, alt] = pot_.Process(put);

		// sum main pot function and its associated CV
		main -= cv_.Process();
		main = main.clip(0_f, 1_f);
		main = filter_.Process(main);

		return std::pair(main, alt);
	}

	f last() {
		return filter_.last();
	}
};

struct NoFilter {
	f Process(f x) {
		return x;
	}
};

template<int block_size>
class Control : public EventSource<Event> {

	Adc adc_;
	SpiAdc spi_adc_;

	Gates gates_;

	struct CalibrationData {
		f pitch_offset;
		f pitch_slope;
		f root_offset;
		f root_slope;
		f warp_offset;
		f balance_offset;
		f twist_offset;
		f scale_offset;
		f modulation_offset;
		f spread_offset;

		// on load, checks that calibration data are within bounds
		bool validate() {
			return true;
		}
	};
	CalibrationData calibration_data_;
	CalibrationData default_calibration_data_ = {
		.pitch_offset = 0.5_f,
		.pitch_slope = -120._f,
		.root_offset = 0.5_f,
		.root_slope = -120._f,
		.warp_offset = 0._f,
		.balance_offset = 0._f,
		.twist_offset = 0._f,
		.scale_offset = 0._f,
		.modulation_offset = 0._f,
		.spread_offset = 0._f,
	};

	Persistent<WearLevel<FlashBlock<0, CalibrationData>>> calibration_data_storage_{&calibration_data_,
																					default_calibration_data_};

	template<AdcInput POT,
			 AdcInput CV,
			 typename PotLp = QuadraticOnePoleLp<1>,
			 typename SumFilter = QuadraticOnePoleLp<1>>
	using DefaultPotCV = PotCVCombiner<PotConditioner<POT, Law::LINEAR, PotLp>, CVConditioner<CV>, SumFilter>;

	DefaultPotCV<POT_WARP, CV_WARP> warp_{adc_, calibration_data_.warp_offset};
	DefaultPotCV<POT_BALANCE, CV_BALANCE, QuadraticOnePoleLp<1>, QuadraticOnePoleLp<2>> balance_{
		adc_, calibration_data_.balance_offset};
	DefaultPotCV<POT_TWIST, CV_TWIST> twist_{adc_, calibration_data_.twist_offset};
	DefaultPotCV<POT_SCALE, CV_SCALE, NoFilter> scale_{adc_, calibration_data_.twist_offset};
	DefaultPotCV<POT_MOD, CV_MOD, NoFilter> modulation_{adc_, calibration_data_.modulation_offset};
	DefaultPotCV<POT_SPREAD, CV_SPREAD, NoFilter> spread_{adc_, calibration_data_.spread_offset};

	PotConditioner<POT_PITCH, Law::LINEAR, QuadraticOnePoleLp<1>> pitch_pot_;
	PotConditioner<POT_ROOT, Law::LINEAR, QuadraticOnePoleLp<1>> root_pot_{adc_};

	PotCVCombiner<PotConditioner<POT_DETUNE, Law::LINEAR, NoFilter>, NoCVInput, QuadraticOnePoleLp<1>> detune_{adc_};

	ExtCVConditioner<CV_PITCH, Average<2, 2>> pitch_cv_{
		calibration_data_.pitch_offset, calibration_data_.pitch_slope, spi_adc_};

	ExtCVConditioner<CV_ROOT, Average<2, 2>> root_cv_{
		calibration_data_.root_offset, calibration_data_.root_slope, spi_adc_};

	Parameters &params_;
	PolypticOscillator<block_size> &osc_;

	easiglib::Sampler<f> pitch_cv_sampler_;

	uint8_t ext_cv_chan;

public:
	Control(Parameters &params, PolypticOscillator<block_size> &osc)
		: pitch_pot_(adc_) //, params.alt.pitch_pot_state)
		, params_(params)
		, osc_(osc) {
	}

	void ProcessSpiAdcInput() {
		if (ext_cv_chan) {
			pitch_cv_.Process();
			pitch_cv_.switch_channel();
		} else {
			root_cv_.Process();
			root_cv_.switch_channel();
		}
		ext_cv_chan = !ext_cv_chan;
	}

	void Poll(std::function<void(Event)> const &put) final {

		// Process gates
		gates_.Debounce();

		if (gates_.freeze_.just_enabled()) {
			osc_.set_freeze(!osc_.frozen());
		} else if (gates_.freeze_.just_disabled()) {
			osc_.set_freeze(!osc_.frozen());
		}

		if (gates_.learn_.just_enabled() && osc_.learn_mode()) {
			put({NewNoteAfterDelay, 0});
		}

		// Process potentiometer & CV

		// DETUNE
		{
			f detune = detune_.Process(put);
			detune = (detune * detune) * (detune * detune);
			detune *= 10_f / f(kMaxNumOsc);
			params_.detune = detune;
		}

		// BALANCE
		{
			// auto [balance, crossfade] = balance_.ProcessDualFunction(put);
			auto balance = balance_.Process(put);

			balance = balance * 2_f - 1_f;		// -1..1
			balance *= balance * balance;		// -1..1 cubic
			balance *= 4_f;						// -4..4
			balance = Math::fast_exp2(balance); // 0.0625..16
			params_.balance = balance;

			auto crossfade = crossfade_raw;
			if (crossfade > 0_f) {
				crossfade *= crossfade;					  // 0..1
				crossfade = 1_f - crossfade;			  // 0..1
				crossfade *= 0.5_f;						  // 0..0.5
				params_.alt.crossfade_factor = crossfade; // 0..0.5
			}
		}

		// TWIST
		{
			auto twist = twist_.Process(put);

			// avoids CV noise to produce harmonics near 0
			twist = Signal::crop_down(0.01_f, twist);
			// scaling of Twist
			if (params_.twist.mode == FEEDBACK) {
				twist *= twist * 0.7_f;
			} else if (params_.twist.mode == PULSAR) {
				twist *= twist;
				// Warning: when changing this constant, also change Pulsar
				// distortion function
				twist = Math::fast_exp2(twist * 6_f); // 1..2^6
			} else if (params_.twist.mode == CRUSH) {
				twist *= twist * 0.5_f;
			}
			params_.twist.value = twist;

			params_.alt.freeze_mode = freeze_mode_raw;
			// if (freeze_mode > 0_f) {
			// 	freeze_mode *= 3_f; // 3 split modes
			// 	SplitMode m = static_cast<SplitMode>(freeze_mode.floor());
			// 	if (m != params_.alt.freeze_mode)
			// 		put({AltParamChange, m});
			// 	params_.alt.freeze_mode = m;
			// }
		}

		// WARP
		{
			// auto [warp, stereo_mode] = warp_.ProcessDualFunction(put);
			auto warp = warp_.Process(put);

			// avoids CV noise to produce harmonics near 0
			warp = Signal::crop_down(0.01_f, warp);
			if (params_.warp.mode == FOLD) {
				warp *= warp;
				warp *= 0.9_f;
				// the little offset avoids scaling the input too close to
				// zero; reducing it makes the wavefolder more linear around
				// warp=0, but increases the quantization noise.
				warp += 0.004_f;
			} else if (params_.warp.mode == CHEBY) {
			} else if (params_.warp.mode == SEGMENT) {
			}
			params_.warp.value = warp;

			params_.alt.stereo_mode = stereo_mode_raw;
			// if (stereo_mode > 0_f) {
			// 	stereo_mode *= 3_f; // 3 split modes
			// 	SplitMode m = static_cast<SplitMode>(stereo_mode.floor());
			// 	if (m != params_.alt.stereo_mode)
			// 		put({AltParamChange, m});
			// params_.alt.stereo_mode = m;
			// }
		}

		// MODULATION
		{
			f mod = modulation_.Process(put);
			// avoids CV noise to produce harmonics near 0
			mod = Signal::crop_down(0.01_f, mod);
			mod *= 6_f / f(params_.alt.numOsc);
			if (params_.modulation.mode == ONE) {
				mod *= 6.0_f;
			} else if (params_.modulation.mode == TWO) {
				mod *= 0.9_f;
			} else if (params_.modulation.mode == THREE) {
				mod *= 4.0_f;
			}
			params_.modulation.value = mod;
		}

		// SPREAD
		{
			auto spread = spread_.Process(put);

			spread *= 10_f / f(kMaxNumOsc);
			params_.spread = spread * kSpreadRange;

			params_.alt.numOsc = num_osc_raw;
			// if (numOsc > 0_f) {
			// 	numOsc *= f(kMaxNumOsc - 1); // [0..max]
			// 	numOsc += 1.5_f;			 // [1.5..max+0.5]
			// 	int n = numOsc.floor();		 // [1..max]
			// 	if (n != params_.alt.numOsc)
			// 		put({AltParamChange, n});
			// 	params_.alt.numOsc = n;
			// }
		}

		// SCALE
		{
			f scale = scale_.Process(put);
			scale *= 9_f;	// [0..9]
			scale += 0.5_f; // [0.5..9.5]
			int g = scale.floor();
			if (g != params_.scale.value)
				put({ScaleChange, g});
			params_.scale.value = g; // [0..9]
		}

		// PITCH
		{
			auto pitch = pitch_pot_.Process(put);

			pitch *= kPitchPotRange;		 // 0..range
			pitch -= kPitchPotRange * 0.5_f; // -range/2..range/2
			f pitch_cv = pitch_cv_.last();

			pitch_cv = pitch_cv_sampler_.Process(pitch_cv);
			pitch += pitch_cv;

			params_.fine_tune = fine_tune_raw > 0_f ? (fine_tune_raw - 0.5_f) * kFineTuneRange : 0_f;
			params_.pitch = pitch + params_.fine_tune;
		}

		// ROOT
		{
			auto root = root_pot_.Process(put);
			f new_note = 0_f;
			// TODO: handle manually dialing in notes?

			root *= kRootPotRange;
			root += root_cv_.last();

			params_.root = root.max(0_f);

			if (new_note > 0_f) {
				new_note *= kRootPotRange;
				//Root CV is allowed to modify manually learned notes
				//so that if a keyboard/seq is patched into the jack,
				//the learn'ed pitches are consistant.
				new_note += root_cv_.last();
				params_.new_note = new_note.max(0_f);
			}
		}
	}

	f pitch_cv() {
		return pitch_cv_.last();
	}
	void hold_pitch_cv() {
		pitch_cv_sampler_.hold();
	}
	void release_pitch_cv() {
		pitch_cv_sampler_.release();
	}

	void spread_pot_alternate_function() {
		// spread_.pot_.alt();
	}
	void spread_pot_main_function() {
		// spread_.pot_.main();
	}
	void root_pot_alternate_function() {
		// root_pot_.alt();
	}
	void root_pot_main_function() {
		// root_pot_.main();
	}
	void pitch_pot_alternate_function() {
		// pitch_pot_.alt();
	}
	void pitch_pot_main_function() {
		// pitch_pot_.main();
	}
	void pitch_pot_reset_alternate_value() {
		// pitch_pot_.reset_alt_value();
	}
	void twist_pot_alternate_function() {
		// twist_.pot_.alt();
	}
	void twist_pot_main_function() {
		// twist_.pot_.main();
	}
	void warp_pot_alternate_function() {
		// warp_.pot_.alt();
	}
	void warp_pot_main_function() {
		// warp_.pot_.main();
	}
	void balance_pot_alternate_function() {
		// balance_.pot_.alt();
	}
	void balance_pot_main_function() {
		// balance_.pot_.main();
	}

	// auto pitch_pot_state() {
	// 	return pitch_pot_.get_state();
	// }

	void disable_all_alt_shift_pot_values() {
		// spread_.pot_.disable();
		// twist_.pot_.disable();
		// warp_.pot_.disable();
		// balance_.pot_.disable();
		// pitch_pot_.disable();
	}

	void disable_all_alt_learn_pot_values() {
		// root_pot_.disable();
		// pitch_pot_.disable();
	}

	f scale_pot() {
		return scale_.pot_.raw();
	}
	f balance_pot() {
		return balance_.pot_.raw();
	}
	f twist_pot() {
		return twist_.pot_.raw();
	}
	f pitch_pot() {
		return pitch_pot_.raw();
	}
	f modulation_pot() {
		return modulation_.pot_.raw();
	}
	f warp_pot() {
		return warp_.pot_.raw();
	}

	void all_main_function() {
		spread_pot_main_function();
		root_pot_main_function();
		pitch_pot_main_function();
		twist_pot_main_function();
		warp_pot_main_function();
		balance_pot_main_function();
		pitch_pot_main_function();
	}

	// Hardware model setters:

	void set_potcv(AdcInput chan, float val) {
		adc_.set(chan, u0_16::inclusive(f(val).clip(0._f, 1._f)));
	}

	void set_pitchroot_cv(SpiAdcInput chan, float val) {
		spi_adc_.set(chan, u0_16::inclusive(f(val).clip(0._f, 1._f)));
	}

	void set_gate(Gate gatenum, bool val) {
		if (gatenum == GATE_FREEZE)
			gates_.freeze_.set(val);
		if (gatenum == GATE_LEARN)
			gates_.learn_.set(val);
	}

	f fine_tune_raw = 0_f;
	f crossfade_raw = 0.4_f;
	int num_osc_raw = 16;
	SplitMode freeze_mode_raw = SplitMode::ALTERNATE;
	SplitMode stereo_mode_raw = SplitMode::ALTERNATE;

	void set_fine_tune(float val) {
		fine_tune_raw = f(val);
	}

	void set_crossfade(float val) {
		crossfade_raw = f(val);
	}

	void set_num_osc(int val) {
		num_osc_raw = val;
	}

	void set_freeze_mode(SplitMode val) {
		freeze_mode_raw = val;
	}

	void set_stereo_mode(SplitMode val) {
		stereo_mode_raw = val;
	}
};

}
