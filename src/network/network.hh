#pragma once
#include <network.hpp>
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

// Requests are serialized: only one runs at a time
std::vector<uint8_t> requestRaw(rack::network::Method method,
								const std::string &url,
								const std::span<uint8_t> &reqStr,
								const rack::network::CookieMap &cookies = {},
								const RequestOptions &options = {});

}
