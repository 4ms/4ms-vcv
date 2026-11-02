#pragma once
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace MetaModule::FlatBuffers
{
std::vector<uint8_t> encode_file(std::string_view filename, std::string_view contents, std::string_view volume);
std::pair<bool, std::string> decode_response(std::vector<uint8_t> rawmsg);

struct WifiStatus {
	uint8_t cpu_load;
	bool playing;
	bool operator==(WifiStatus const &) const = default;
};

std::vector<uint8_t> encode_status_request();
std::optional<WifiStatus> decode_status_response(std::span<const uint8_t> rawmsg);
} // namespace MetaModule::FlatBuffers
