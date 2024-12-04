#include <rack.hpp>
#include <string>

namespace MetaModule
{

std::string get_volume_host_path() {
	return rack::asset::user("");
}

} // namespace MetaModule
