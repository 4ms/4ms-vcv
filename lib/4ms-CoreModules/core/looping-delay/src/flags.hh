#pragma once
#include <cstdint>

namespace LDKit
{
struct Flags {
private:
	// TODO: make these atomic, if necessary
	bool _time_changed = true;
	bool _inf_changed = false;
	bool _rev_changed = false;
	bool _time_quantized_change = false;
	bool _inf_quantized_change = false;
	bool _rev_quantized_change = false;
	float _scroll_loop_amt = 0.f;
	bool _clear_memory = false;

public:
	// TODO: This has hardcoded sample rate
	uint32_t mute_on_boot_ctr = 375; // 0.250s * 48000 / 32 block-size

	bool take_time_changed() {
		auto t = _time_changed;
		_time_changed = false;
		return t;
	}
	bool take_inf_changed() {
		auto t = _inf_changed;
		_inf_changed = false;
		return t;
	}
	bool take_rev_changed() {
		auto t = _rev_changed;
		_rev_changed = false;
		return t;
	}
	bool take_time_quantized_changed() {
		auto t = _time_quantized_change;
		_time_quantized_change = false;
		return t;
	}
	bool take_inf_quantized_changed() {
		auto t = _inf_quantized_change;
		_inf_quantized_change = false;
		return t;
	}
	bool take_rev_quantized_changed() {
		auto t = _rev_quantized_change;
		_rev_quantized_change = false;
		return t;
	}
	float take_scroll_amt() {
		float amt = _scroll_loop_amt;
		_scroll_loop_amt = 0.f;
		return amt;
	}
	bool take_clear_memory() {
		bool t = _clear_memory;
		_clear_memory = false;
		return t;
	}

	void set_time_changed() {
		_time_changed = true;
	}
	void set_inf_changed() {
		_inf_changed = true;
	}
	void set_rev_changed() {
		_rev_changed = true;
	}
	void set_time_quantized_changed() {
		_time_quantized_change = true;
	}
	void set_inf_quantized_changed() {
		_inf_quantized_change = true;
	}
	void set_rev_quantized_changed() {
		_rev_quantized_change = true;
	}
	void set_scroll_amt(float amt) {
		_scroll_loop_amt = amt;
	}
	void add_scroll_amt(float amt) {
		_scroll_loop_amt += amt;
	}
	void set_clear_memory() {
		_clear_memory = true;
	}
};
} // namespace LDKit
