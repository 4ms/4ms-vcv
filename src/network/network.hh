#include <network.hpp>
#include <string>

namespace MetaModule::network
{

std::string requestRaw(rack::network::Method method,
					   const std::string &url,
					   const std::string &reqStr,
					   const rack::network::CookieMap &cookies = {});

}
