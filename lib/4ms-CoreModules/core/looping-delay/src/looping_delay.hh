#pragma once
#include "audio_stream_conf.hh"
#include "auto_mute.hh"
#include "compress.hh"
#include "controls.hh"
#include "dcblock.hh"
#include "epp_lut.hh"
#include "flags.hh"
#include "loop_util.hh"
#include "params.hh"
#include "util/circular_buffer_ext.hh"
#include "util/math.hh"
#include "util/zip.hh"
#include <algorithm>

#include "../mocks/delay_buffer.hh"

namespace LDKit
{

class LoopingDelay {
public:
	static constexpr uint32_t DefaultSampleRate = 48000;

private:
	Params &params;
	Flags &flags;

	DelayBuffer delay_buffer;

	CircularBufferAccess<DelayBuffer::span> buf;
	CircularBufferAccess<DelayBuffer::span> fade_buf;

	float read_fade_phase = 0;
	float queued_divmult_time = 0;
	uint32_t queued_read_fade_ending_addr = 0;

	float write_fade_phase = 0.f;
	enum class FadeState { NotFading, FadingDown, FadingUp, Crossfading };
	FadeState write_fade_state = FadeState::NotFading; // write_fade_state

	bool doing_reverse_fade = false;

	uint32_t loop_start = 0;
	uint32_t loop_end = 0;

	static constexpr float AttackTimeInS = 0.020f;						  // 20ms
	static constexpr float DecayTimeInS = 0.020f;						  // 20ms
	static constexpr int32_t AutoMuteThreshold = 0.020 / 20. * 0x7F'FFFF; // 20mV of 20Vpp

	static constexpr int32_t AutoMuteAttack = AttackTimeInS * float(DefaultSampleRate);
	static constexpr int32_t AutoMuteDecay = DecayTimeInS * float(DefaultSampleRate);

	AutoMute<500, AutoMuteThreshold, AutoMuteAttack, AutoMuteDecay> main_automute;
	AutoMute<500, AutoMuteThreshold, AutoMuteAttack, AutoMuteDecay> aux_automute;

	DCBlock<4800, int32_t> dcblock;

public:
	LoopingDelay(Params &params, Flags &flags)
		: params{params}
		, flags{flags}
		, buf{delay_buffer.get(), static_cast<size_t>(params.divmult_time)}
		, fade_buf{delay_buffer.get(), static_cast<size_t>(params.divmult_time)} {
		delay_buffer.clear();
	}

	void set_samplerate(uint32_t newSampleRate) {
		if (newSampleRate != params.currentSampleRate) {
			main_automute.setAttackPeriod(AttackTimeInS * float(newSampleRate));
			main_automute.setDecayPeriod(DecayTimeInS * float(newSampleRate));
			aux_automute.setAttackPeriod(AttackTimeInS * float(newSampleRate));
			aux_automute.setDecayPeriod(DecayTimeInS * float(newSampleRate));

			delay_buffer.clear();

			params.timer.scale_periods((float)newSampleRate / params.currentSampleRate);
			flags.set_time_changed();

			params.currentSampleRate = newSampleRate;
		}
	}

	void set_ping_time(uint32_t ping_time) {
		params.timer.set_ping_time(ping_time);
		flags.set_time_changed();
	}

	float get_ping_time_at_default_samplerate() {
		uint32_t scaled_ping_time = std::round(params.ping_time * DefaultSampleRate / params.currentSampleRate );
		return scaled_ping_time;
	}

	// TODO: when global_mode[CALIBRATE] is set, we should change the audio callback
	// GCC_OPTIMIZE_OFF
	void update(const AudioStreamConf::AudioInBlock &inblock, AudioStreamConf::AudioOutBlock &outblock) {
		if (float amt = flags.take_scroll_amt(); amt != 0.f) {
			scroll_loop(amt);
		}

		if (flags.take_time_changed()) {
			set_divmult_time();
		}

		if (write_fade_state == FadeState::NotFading) {
			if (flags.take_inf_changed())
				toggle_inf();
		}

		if (!doing_reverse_fade) {
			if (flags.take_rev_changed())
				toggle_rev();
		}
		if (flags.take_clear_memory())
			delay_buffer.clear();

		// Buffers for R/W this block (backing data)
		std::array<int16_t, AudioStreamConf::BlockSize * 2> full_rd_buff;
		std::array<int16_t, AudioStreamConf::BlockSize * 2> full_rd_fade_buff;
		std::array<int16_t, AudioStreamConf::BlockSize * 2> full_wr_buff;

		// Stereo mode: use BlockSize*2 elements (interleaved channels)
		std::span<int16_t> rd_buff{full_rd_buff};
		std::span<int16_t> rd_fade_buff{full_rd_fade_buff};
		std::span<int16_t> wr_buff{full_wr_buff};

		// Mono mode: use BlockSize elements
		if (!params.settings.stereo_mode) {
			rd_buff = rd_buff.first(AudioStreamConf::BlockSize);
			wr_buff = wr_buff.first(AudioStreamConf::BlockSize);
			rd_fade_buff = rd_fade_buff.first(AudioStreamConf::BlockSize);
		}

		// Read into rd_buff:
		bool read_reverse = doing_reverse_fade ^ params.modes.reverse;
		if (params.modes.inf == InfState::Off) {
			check_read_write_head_spacing();
			read_reverse ? buf.read_reverse(rd_buff) : buf.read(rd_buff);
		} else {
			if (!is_crossfading()) {
				if (!check_read_head_in_loop()) {
					start_crossfade(loop_start);
				}
			}

			auto marker = calc_start_fade_addr();
			bool did_cross_start_fade = read_reverse ? buf.read_reverse_check_crossed(rd_buff, marker) :
													   buf.read_check_crossed(rd_buff, marker);

			if (did_cross_start_fade) {
				start_looping_crossfade();
			}
		}

		// Read into crossfading buffer (TODO: shouldn't this only happen if we're xfading?)
		params.modes.reverse ? fade_buf.read_reverse(rd_fade_buff) : fade_buf.read(rd_fade_buff);

		for (unsigned i = 0; i < AudioStreamConf::BlockSize; i++) {
			bool mono = !params.settings.stereo_mode;

			// Inputs
			const int16_t mem_rd_l = mono ? rd_buff[i] : rd_buff[i * 2];
			const int16_t mem_rd_r = mono ? 0 : rd_buff[i * 2 + 1];

			const int16_t mem_rd_fade_l = mono ? rd_fade_buff[i] : rd_fade_buff[i * 2];
			const int16_t mem_rd_fade_r = mono ? 0 : rd_fade_buff[i * 2 + 1];

			auto auxin = flags.mute_on_boot_ctr ? 0 : AudioStreamConf::AudioInFrame::sign_extend(inblock[i].chan[0]);
			auto mainin = flags.mute_on_boot_ctr ? 0 : AudioStreamConf::AudioInFrame::sign_extend(inblock[i].chan[1]);

			// Outputs
			int16_t nul;
			auto &mem_wr_l = mono ? wr_buff[i] : wr_buff[i * 2];
			auto &mem_wr_r = mono ? nul : wr_buff[i * 2 + 1];
			auto &out = outblock[i];

			if (params.settings.auto_mute) {
				mainin = main_automute.update(mainin);
				auxin = aux_automute.update(auxin);
			}

			// Crossfade the two read head positions
			int32_t rd_l = epp_crossfade<int32_t>(mem_rd_l, mem_rd_fade_l, read_fade_phase);
			int32_t rd_r = epp_crossfade<int32_t>(mem_rd_r, mem_rd_fade_r, read_fade_phase);

			increment_read_crossfading();

			// 16 bit => 24 bit
			rd_l *= 256;
			rd_r *= 256;

			// Attenuate the delayed signal with REGEN
			int32_t regen_l = (float)rd_l * params.feedback;
			int32_t regen_r = (float)rd_r * params.feedback;

			// Attenuate the clean signal by the LEVEL parameter
			int32_t mainin_atten_l = (float)mainin * params.delay_feed;
			int32_t mainin_atten_r = (float)auxin * params.delay_feed;

			int32_t wr_l;
			int32_t wr_r;
			if (params.settings.stereo_mode) {
				wr_l = (int32_t)(regen_l + mainin_atten_l);
				wr_r = (int32_t)(regen_r + mainin_atten_r);
				// } else if (params.settings.send_return_before_loop) {
				// 	wr = auxin;
				// 	auxout = (int32_t)(regen + mainin_atten);
			} else {
				wr_l = (int32_t)(regen_l + mainin_atten_l + (float)auxin);
				wr_r = 0;
			}

			// Wet/dry mix, as determined by the MIX parameter
			int32_t mix_l = ((float)mainin * params.mix_dry) + ((float)rd_l * params.mix_wet);
			int32_t mix_r = ((float)auxin * params.mix_dry) + ((float)rd_r * params.mix_wet);

			out.chan[0] = mono ? clip(Brain::AudioGain * rd_l) : clip(Brain::AudioGain * mix_r);
			out.chan[1] = clip(Brain::AudioGain * mix_l);

			// High-pass filter before writing to memory
			// if (params.settings.runaway_dc_block) {
			wr_l = dcblock.update(wr_l);
			wr_r = dcblock.update(wr_r);
			// }
			mem_wr_l = clip(wr_l) / 256;
			if (!mono)
				mem_wr_r = clip(wr_r) / 256;
		}

		write_block_to_memory(wr_buff);

		handle_read_crossfade_end();
		increment_write_crossfading();
	}

	void write_block_to_memory(std::span<int16_t> wr_buff) {
		if (params.modes.inf == InfState::On)
			return;

		bool rev = params.modes.reverse;

		if (params.modes.inf == InfState::TransitioningOn) {
			if (write_fade_state == FadeState::FadingDown) {
				float phase = 1.f - write_fade_phase;
				rev ? fade_buf.write_reverse(wr_buff, phase) : fade_buf.write(wr_buff, phase);
				buf.wr_pos(fade_buf.wr_pos());
			}
		}

		if (params.modes.inf == InfState::TransitioningOff || params.modes.inf == InfState::Off) {
			if (write_fade_state == FadeState::Crossfading) {
				// Memory::fade_write(write_fade_ending_addr, wr_buff, rev, write_fade_phase);
				rev ? fade_buf.write_reverse(wr_buff, write_fade_phase) : fade_buf.write(wr_buff, write_fade_phase);

				// write in the opposite direction of rev
				rev ? fade_buf.write(wr_buff, 1.f - write_fade_phase) :
					  fade_buf.write_reverse(wr_buff, 1.f - write_fade_phase);
			} else if (write_fade_state == FadeState::FadingUp) {
				rev ? fade_buf.write_reverse(wr_buff, write_fade_phase) : fade_buf.write(wr_buff, write_fade_phase);
				buf.wr_pos(fade_buf.wr_pos());
			} else {
				rev ? buf.write_reverse(wr_buff) : buf.write(wr_buff);
				fade_buf.wr_pos(buf.wr_pos());
			}
		}
	}

	bool check_read_head_in_loop() {
		// If we're not crossfading, check if the read head is inside the loop
		return Util::in_between(buf.rd_pos(), loop_start, loop_end, params.modes.reverse);
	}

	void check_read_write_head_spacing() {
		if (!is_crossfading()) {
			auto t = calculate_read_addr(params.divmult_time);
			if (t != buf.rd_pos())
				set_divmult_time();
		}
	}

	// Returns the address where we should start crossfading before looping
	uint32_t calc_start_fade_addr() {
		// For short periods (audio rate), disble crossfading before the end of the loop
		if (params.divmult_time < params.settings.crossfade_samples)
			return loop_end;
		else {
			auto offset = params.settings.crossfade_samples;
			if (params.settings.stereo_mode)
				offset *= 2;
			return Util::offset_samples(loop_end, offset, !params.modes.reverse);
		}
	}

	//  When we near the end of the loop, start a crossfade to the beginning
	void start_looping_crossfade() {
		if (params.divmult_time <= params.settings.crossfade_samples) {
			buf.rd_pos(loop_start);
			read_fade_phase = 0.f;

			// Issue: is it necessary to set this below?
			fade_buf.rd_pos(Util::offset_samples(buf.rd_pos(), AudioStreamConf::BlockSize, !params.modes.reverse));
		} else {
			// Start fading from before the loop
			// We have to add in sz because read_addr has already
			// been incremented by sz since a block was just read
			int32_t loop_size = loop_end - loop_start;
			// ensure stereo frame alignment
			if (params.settings.stereo_mode)
				loop_size &= ~1U;
			if (params.modes.reverse)
				loop_size = -loop_size;

			uint32_t f_addr = Util::offset_samples(buf.rd_pos(), loop_size, !params.modes.reverse);

			start_crossfade(f_addr);
		}

		if (params.modes.inf == InfState::TransitioningOff)
			params.set_inf_state(InfState::Off);
	}

	int32_t clip(int32_t val) {
		constexpr size_t Max24bit = (1U << 23) - 1;
		if (params.settings.soft_clip)
			val = compress<Max24bit, 75>(val);
		else
			val = MathTools::signed_saturate(val, 24);
		return val;
	}

	uint32_t calculate_read_addr(uint32_t divmult_time) {
		if (params.settings.stereo_mode)
			divmult_time *= 2;
		return Util::offset_samples(buf.wr_pos(), divmult_time, !params.modes.reverse);
	}

	void set_divmult_time() {
		uint32_t use_ping_time = params.modes.ping_locked ? params.locked_ping_time : params.ping_time;
		float t_divmult_time = use_ping_time * params.time;
		t_divmult_time = std::clamp(t_divmult_time, 0.f, (float)MemorySamplesNum);

		// Crossfade to new read head position (or queue it if we're already crossfading)
		if (params.modes.inf == InfState::Off) {
			if (is_crossfading()) {
				if (params.divmult_time != t_divmult_time)
					queued_divmult_time = t_divmult_time;
			} else {
				params.set_divmult(t_divmult_time);
				uint32_t t_read_addr = calculate_read_addr(params.divmult_time);
				if (t_read_addr != buf.rd_pos())
					start_crossfade(t_read_addr);
			}
		} else {
			params.set_divmult(t_divmult_time);

			if (params.settings.stereo_mode)
				t_divmult_time *= 2;

			if (params.modes.adjust_loop_end)
				loop_end = Util::offset_samples(loop_start, t_divmult_time, params.modes.reverse);
			else
				loop_start = Util::offset_samples(loop_end, t_divmult_time, !params.modes.reverse);

			// If the read addr is not in between the loop start and end, then fade to the loop start
			if (!check_read_head_in_loop()) {
				if (is_crossfading()) {
					queued_read_fade_ending_addr = loop_start;
				} else {
					start_crossfade(loop_start);
				}
			}
		}
	}

	bool is_crossfading() {
		return read_fade_phase >= params.settings.crossfade_rate;
	}

	void start_crossfade(uint32_t addr) {
		read_fade_phase = params.settings.crossfade_rate;
		queued_divmult_time = 0; // means: no queued crossfade
		fade_buf.rd_pos(addr);
	}

	void increment_read_crossfading() {
		if (read_fade_phase > 0.0f) {
			read_fade_phase += params.settings.crossfade_rate;
			if (read_fade_phase > 1.f) {
				read_fade_phase = 1.f;
			}
		}
	}

	void handle_read_crossfade_end() {
		if (read_fade_phase >= 1.f) {
			read_fade_phase = 0.f;
			doing_reverse_fade = false;
			buf.rd_pos(fade_buf.rd_pos());

			if (queued_divmult_time > 0.f) {
				params.set_divmult(queued_divmult_time);
				start_crossfade(calculate_read_addr(queued_divmult_time));
			} else if (queued_read_fade_ending_addr) {
				start_crossfade(queued_read_fade_ending_addr);
				queued_read_fade_ending_addr = 0;
			}
		}
	}

	void increment_write_crossfading() {
		if (write_fade_phase > 0.f) {
			if (write_fade_state == FadeState::FadingUp)
				write_fade_phase += params.settings.write_crossfade_rate;

			else if (write_fade_state == FadeState::FadingDown)
				write_fade_phase += params.settings.crossfade_rate;

			else if (write_fade_state == FadeState::Crossfading)
				write_fade_phase += params.settings.write_crossfade_rate;

			if (write_fade_phase > 1.f) {
				write_fade_phase = 0.f;
				write_fade_state = FadeState::NotFading;
				buf.wr_pos(fade_buf.wr_pos());
				// write_head = write_fade_ending_addr;
				if (params.modes.inf == InfState::TransitioningOn)
					params.set_inf_state(InfState::On);
			}
		}
	}

	void toggle_rev() {
		params.toggle_reverse();
		if (params.modes.inf == InfState::Off)
			swap_read_write();
		else
			reverse_loop();

		doing_reverse_fade = true;
	}

	// When reversing in INF mode, swap the loop start/end
	// but offset them by the settings.crossfade_samples so the crossfade
	// stays within already recorded audio
	void reverse_loop() {
		uint32_t t = loop_start;
		uint32_t padding = params.settings.crossfade_samples;
		if (params.settings.stereo_mode)
			padding *= 2;

		loop_start = Util::offset_samples(loop_end, padding, params.modes.reverse);
		loop_end = Util::offset_samples(t, padding, params.modes.reverse);

		start_crossfade(buf.rd_pos());
	}

	// When starting a reverse, crossfade a swap of the read/write heads
	void swap_read_write() {
		start_crossfade(fade_buf.wr_pos());
		fade_buf.wr_pos(buf.rd_pos());
		write_fade_phase = params.settings.write_crossfade_rate;
		write_fade_state = FadeState::Crossfading;
	}

	void toggle_inf() {
		if (params.modes.inf == InfState::On || params.modes.inf == InfState::TransitioningOn) {
			params.set_inf_state(InfState::TransitioningOff);

			write_fade_phase = params.settings.write_crossfade_rate;
			write_fade_state = FadeState::FadingUp;
			fade_buf.wr_pos(buf.rd_pos());
			// write_fade_ending_addr = read_head;
		} else {
			// Don't change the loop start/end if we hit INF off recently
			// (recent enough that we're still TransitioningOff)
			// This is because the read and write heads are in the same spot
			if (params.modes.inf != InfState::TransitioningOff) {
				params.reset_loop();

				loop_start = fade_buf.rd_pos(); // read_fade_ending_addr;
				// set loop_start to the fade ending addr because if we happen to
				// be fading the read head when we hit inf (e.g. changing divmult time)
				// then we should loop between the new points since divmult_time
				// (used in the next line) corresponds with the fade ending addr

				auto offset = params.divmult_time;
				if (params.settings.stereo_mode)
					offset *= 2;
				loop_end = Util::offset_samples(loop_start, offset, params.modes.reverse);
			}
			write_fade_phase = params.settings.crossfade_rate;
			write_fade_state = FadeState::FadingDown;
			// write_fade_ending_addr = write_head;
			fade_buf.wr_pos(buf.wr_pos());

			params.set_inf_state(InfState::TransitioningOn);
		}
	}

	void scroll_loop(float amt) {
		uint32_t hi = loop_end;
		uint32_t lo = loop_start;
		if (params.modes.reverse)
			std::swap(hi, lo);

		uint32_t loop_length = (hi > lo) ? (hi - lo) : hi + (Brain::MemorySizeBytes / MemorySampleSize - lo);
		int32_t loop_shift = (int32_t)(amt * (float)loop_length);
		// maintain stereo frame alignment
		if (params.settings.stereo_mode)
			loop_shift &= ~1U;
		loop_start = Util::offset_samples(loop_start, loop_shift);
		loop_end = Util::offset_samples(loop_end, loop_shift);
	}
};

} // namespace LDKit
