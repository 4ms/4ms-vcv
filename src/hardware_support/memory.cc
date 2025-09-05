#include <cstdint>

namespace MetaModule::System
{

uint32_t total_memory() {
	return 512 * 1024 * 1024; //512MB
}

uint32_t free_memory() {
	return total_memory(); //pretend we have infinite memory in simulator???
}

} // namespace MetaModule::System
