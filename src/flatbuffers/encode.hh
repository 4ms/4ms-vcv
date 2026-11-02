#pragma once
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace MetaModule::FlatBuffers
{
std::vector<uint8_t> encode_file(std::string_view filename,
								 std::string_view contents,
								 std::string_view volume,
								 bool load_after_save = false);
std::pair<bool, std::string> decode_response(std::vector<uint8_t> rawmsg);

// The Wi-Fi Expander answered, but the MetaModule didn't (e.g. its firmware doesn't know the request)
bool is_bridge_timeout(std::span<const uint8_t> rawmsg);

// The MetaModule will load and play the patch it saved (false from firmware without this feature)
bool decode_load_requested(std::span<const uint8_t> rawmsg);

struct WifiStatus {
	uint8_t cpu_load;
	bool playing;

	// Only when requested with include_modules:
	std::string patch_name{};
	std::vector<uint16_t> module_loads{}; // tenths of a percent, index = module id in the patch
	uint16_t overhead = 0;				  // tenths of a percent
	bool module_loads_valid = false;
	std::vector<uint8_t> module_cores{}; // core each module runs on (empty: no load balancing info)
	std::vector<uint16_t> core_loads{};	 // each core's modules + cables, tenths of a percent
	uint32_t patch_fingerprint = 0; // of the playing patch, 0 if not sent

	// Not part of the reply: whether the request asked for per-module loads
	bool modules_requested = false;

	bool operator==(WifiStatus const &) const = default;
};

std::vector<uint8_t> encode_status_request(bool include_modules = false);
std::optional<WifiStatus> decode_status_response(std::span<const uint8_t> rawmsg);
} // namespace MetaModule::FlatBuffers
