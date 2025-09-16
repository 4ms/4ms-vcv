#include <cstdint>
#include <cstdlib>

namespace MetaModule::System
{

bool hardware_random_ready() {
	return true;
}

uint32_t hardware_random() {
	return std::rand(); //fake it for the simulator
}

uint32_t random() {
	return std::rand();
}

} // namespace MetaModule::System
