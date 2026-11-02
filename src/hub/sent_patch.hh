#pragma once
#include "flatbuffers/encode.hh"
#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>

namespace MetaModule
{

// What the Hub last sent to the MetaModule over Wi-Fi, to match the
// MetaModule's per-module loads to the modules in the rack
struct SentPatch {
	std::map<int64_t, uint16_t> id_map; // VCV module id -> module id in the patch (0 = Hub)
	std::set<int64_t> rack_module_ids;	// every module in the rack when the patch was sent
	uint32_t fingerprint = 0;			// patch_fingerprint() of the sent patch
};

enum class SyncState {
	NoData,	   // no reply, nothing playing, or per-module loads not measured yet
	NotSynced, // the MetaModule is playing something other than what's in the rack
	FirmwareUnsupported, // the MetaModule's firmware can't report per-module loads
	Synced,
};

inline SyncState sync_state(std::optional<SentPatch> const &sent,
							std::set<int64_t> const &rack_module_ids,
							std::optional<FlatBuffers::WifiStatus> const &status) {
	// Firmware with per-module loads always sends the playing patch's fingerprint
	if (status && status->modules_requested && status->playing && status->patch_fingerprint == 0)
		return SyncState::FirmwareUnsupported;

	if (!sent)
		return SyncState::NotSynced;

	if (!status || !status->playing || !status->module_loads_valid)
		return SyncState::NoData;

	if (rack_module_ids != sent->rack_module_ids)
		return SyncState::NotSynced;

	// The fingerprint tells if the MetaModule plays the same modules in the same order.
	// The name alone could match a different file, or a patch edited on the MetaModule.
	if (status->patch_fingerprint == 0 || status->patch_fingerprint != sent->fingerprint)
		return SyncState::NotSynced;

	return SyncState::Synced;
}

// Load of a VCV module on the MetaModule, in tenths of a percent.
// nullopt if the module isn't in the sent patch or the reply has no value for it.
inline std::optional<uint16_t>
module_load_for(SentPatch const &sent, FlatBuffers::WifiStatus const &status, int64_t vcv_module_id) {
	auto found = sent.id_map.find(vcv_module_id);
	if (found == sent.id_map.end())
		return std::nullopt;

	auto index = found->second;
	if (index >= status.module_loads.size())
		return std::nullopt;

	return status.module_loads[index];
}

// Core a VCV module runs on in the MetaModule (0 = Core 1).
// nullopt if the module isn't in the sent patch or the patch has no load balancing info.
inline std::optional<uint8_t>
module_core_for(SentPatch const &sent, FlatBuffers::WifiStatus const &status, int64_t vcv_module_id) {
	auto found = sent.id_map.find(vcv_module_id);
	if (found == sent.id_map.end() || found->second >= status.module_cores.size())
		return std::nullopt;

	return status.module_cores[found->second];
}

std::string format_module_load(std::optional<uint16_t> tenths);

// Label under a module: "19.5% C1", "19.5%" if the core isn't known, "—" if there's no value
inline std::string format_module_label(std::optional<uint16_t> tenths, std::optional<uint8_t> core) {
	if (!tenths)
		return format_module_load(std::nullopt);

	auto text = format_module_load(tenths);
	if (core)
		text += " C" + std::to_string(*core + 1);
	return text;
}

// Label under the Hub: "Core 1 20.0%  Core 2 19.7%  Overhead 15.0%"
inline std::string format_hub_loads(FlatBuffers::WifiStatus const &status) {
	std::string text;
	for (auto core = 0u; core < status.core_loads.size(); core++)
		text += "Core " + std::to_string(core + 1) + " " + format_module_load(status.core_loads[core]) + "  ";

	return text + "Overhead " + format_module_load(status.overhead);
}

// "3.2%", or "—" if there's nothing to show
inline std::string format_module_load(std::optional<uint16_t> tenths) {
	if (!tenths)
		return "—";

	return std::to_string(*tenths / 10) + "." + std::to_string(*tenths % 10) + "%";
}

} // namespace MetaModule
