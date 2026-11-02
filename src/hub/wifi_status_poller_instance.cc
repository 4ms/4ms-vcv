#include "network/network.hh"
#include "wifi_status_poller.hh"

namespace MetaModule
{

WifiStatusPoller &WifiStatusPoller::instance() {
	static WifiStatusPoller poller{[](std::string const &url, std::span<uint8_t> body) {
		network::RequestOptions options{
			.bridge_timeout_ms = 2000,
			.total_timeout_ms = 4000,
			.log = false,
		};
		return network::requestRaw(rack::network::METHOD_POST, url + "/action", body, {}, options);
	}};
	return poller;
}

} // namespace MetaModule
