#include <cstdint>
#include <time.h>

namespace MetaModule::System
{

uint32_t get_ticks() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)(ts.tv_nsec / 1'000'000) + ((uint64_t)ts.tv_sec * 1000ull);
}

void delay_ms(uint32_t ms) {
	uint64_t tickstart = get_ticks();
	uint64_t wait = ms + 1;
	while ((get_ticks() - tickstart) < wait)
		;
}

} // namespace MetaModule::System
