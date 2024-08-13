#pragma once
#include "audio_stream_conf.hh"
#include "ping_methods.hh"

namespace LDKit
{
enum class GateType : uint8_t { Gate = 0, Trig = 1 };

// Settings cannot be changed in Normal operation mode
struct Settings {
	bool auto_mute;
	bool soft_clip;
	PingMethod ping_method;
	GateType rev_jack;
	GateType inf_jack;
	GateType loop_clock;
	GateType main_clock;
	bool log_delay_feed;
	bool auto_unquantize_timejack;
	bool send_return_before_loop;
	bool stereo_mode;

	uint32_t crossfade_samples;		  // SLOW_FADE_SAMPLES
	float crossfade_rate;			  // SLOW_FADE_INCREMENT
	uint32_t write_crossfade_samples; // FAST_FADE_SAMPLES
	float write_crossfade_rate;		  // FAST_FADE_INCREMENT

	static constexpr float calc_fade_increment(uint32_t samples) {
		return samples > 0.f ? 1.f / (float)samples : 1.f;
	}
};
} // namespace LDKit
