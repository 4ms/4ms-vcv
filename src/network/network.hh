#pragma once
#include <network.hpp>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace MetaModule::network
{

struct RequestOptions {
	unsigned bridge_timeout_ms = 10000; // how long the Wi-Fi bridge waits for the MetaModule
	long total_timeout_ms = 0;			// 0 = no limit on the whole request
	bool log = true;					// log each request to Rack's log
};

// Requests are serialized: only one runs at a time.
// Blocks until any background request has stopped.
std::vector<uint8_t> requestRaw(rack::network::Method method,
								const std::string &url,
								const std::span<uint8_t> &reqStr,
								const rack::network::CookieMap &cookies = {},
								const RequestOptions &options = {});

// For polling: gives way to requestRaw(). Returns nullopt if skipped because another
// request is running or waiting, or if it was stopped early to let one through.
std::optional<std::vector<uint8_t>> tryRequestRawInBackground(rack::network::Method method,
															   const std::string &url,
															   const std::span<uint8_t> &reqStr,
															   const RequestOptions &options = {});

}
