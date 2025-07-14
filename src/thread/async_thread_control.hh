#pragma once
#include "CoreModules/CoreProcessor.hh"
#include "util/callable.hh"
#include <optional>

namespace MetaModule
{

namespace Async
{

struct Task {
	uint32_t id = 0;

	Callback action{};
	bool enabled = false;
	bool one_shot = false;
};

std::optional<uint32_t> create_task(CoreProcessor *module);
void destroy_task(uint32_t id);

Async::Task *get_task(unsigned id);

void start_module_threads();
void kill_module_threads();

} // namespace Async

} // namespace MetaModule
