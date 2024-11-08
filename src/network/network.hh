#include <network.hpp>
#include <span>
#include <string>

namespace MetaModule::network
{

std::string requestRaw(rack::network::Method method,
					   const std::string &url,
					   const std::span<uint8_t> &reqStr,
					   const rack::network::CookieMap &cookies = {});

}
