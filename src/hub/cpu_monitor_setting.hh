#pragma once
#include <jansson.h>

namespace MetaModule
{

// "Wi-Fi live CPU load monitor" in the Hub's menu
enum class CpuMonitor { Off = 0, Total = 1, TotalAndPerModule = 2 };

inline CpuMonitor read_cpu_monitor_setting(json_t *rootJ) {
	auto *modeJ = json_object_get(rootJ, "wifiCpuMonitor");
	if (!json_is_integer(modeJ))
		return CpuMonitor::Off;

	auto val = json_integer_value(modeJ);
	if (val == (int)CpuMonitor::Total)
		return CpuMonitor::Total;
	if (val == (int)CpuMonitor::TotalAndPerModule)
		return CpuMonitor::TotalAndPerModule;
	return CpuMonitor::Off;
}

inline void write_cpu_monitor_setting(json_t *rootJ, CpuMonitor mode) {
	json_object_set_new(rootJ, "wifiCpuMonitor", json_integer((int)mode));
}

} // namespace MetaModule
