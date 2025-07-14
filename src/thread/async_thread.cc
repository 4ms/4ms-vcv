#include "CoreModules/async_thread.hh"
#include "async_thread_control.hh"
#include "comm/comm_module.hh"
#include <thread>

namespace MetaModule
{
struct AsyncThread::Internal {
	std::optional<uint32_t> id;

	Internal(std::optional<uint32_t> id)
		: id(id) {
	}
};

AsyncThread::AsyncThread(CoreProcessor *module)
	: internal{std::make_unique<Internal>(Async::create_task(module))} {
}

AsyncThread::AsyncThread(CoreProcessor *module, Callback &&new_action)
	: action(std::move(new_action))
	, internal{std::make_unique<Internal>(Async::create_task(module))} {
}

void AsyncThread::start() {
	if (internal->id) {
		if (auto task = Async::get_task(*internal->id)) {
			// printf("Start task id %u\n", *internal->id);

			task->action = action;
			task->one_shot = false;
			task->enabled = true;
		}
	}
}

void AsyncThread::start(Callback &&new_action) {
	action = std::move(new_action);
	start();
}

void AsyncThread::run_once() {
	if (internal->id) {
		if (auto task = Async::get_task(*internal->id)) {
			task->action = action;
			task->one_shot = true;
			task->enabled = true;

			// printf("Run once task id %u\n", *internal->id);
		}
	}
}

void AsyncThread::stop() {
	if (internal->id) {
		if (auto task = Async::get_task(*internal->id)) {
			task->enabled = false;

			// printf("Stop task id %u\n", *internal->id);
		}
		// else
		// printf("Can't stop unknown task id %u\n", *internal->id);
	}
}

AsyncThread::~AsyncThread() {
	stop();
	if (internal->id)
		Async::destroy_task(*internal->id);
}

bool AsyncThread::is_enabled() {
	if (internal->id) {
		if (auto task = Async::get_task(*internal->id)) {
			return task->enabled;
		}
	}
	return false;
}

} // namespace MetaModule
