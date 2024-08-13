#pragma once
#include <cstdint>

namespace LDKit
{
enum class InfState { Off, On, TransitioningOn, TransitioningOff };

struct ChannelMode {
	InfState inf = InfState::Off;
	bool reverse = false;
	bool time_pot_quantized = true;
	bool time_cv_quantized = true;
	bool ping_locked = false;
	bool quantize_mode_changes = false;
	bool adjust_loop_end = false;
};

enum class OperationMode { Normal, SysSettings, Calibrate };
} // namespace LDKit
