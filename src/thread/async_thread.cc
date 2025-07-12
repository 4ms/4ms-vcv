#include "CoreModules/async_thread.hh"
#include <thread>

namespace MetaModule
{

struct AsyncThread::Internal {
	uint32_t id = 0;
	std::atomic<bool> enabled = false;
	std::thread thread;

	Internal(uint32_t id)
		: id{id} {
	}
};

namespace //anonymous
{

std::atomic<bool> kill_signal = false;
std::atomic<bool> pause_signal = false;

} // namespace

AsyncThread::AsyncThread(CoreProcessor *module)
	: internal{std::make_unique<Internal>(module->id)} {
}

AsyncThread::AsyncThread(CoreProcessor *module, Callback &&new_action)
	: action(std::move(new_action))
	, internal{std::make_unique<Internal>(module->id)} {
}

void AsyncThread::start() {
	if (action) {
		printf("Start task id %u\n", internal->id);

		internal->enabled = true;

		internal->thread = std::thread([this]() {
			while (internal->enabled) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));

				if (!pause_signal)
					action();

				if (kill_signal)
					break;
			}

			printf("Task %u: exited running loop.\n", internal->id);
		});
	}
}

void AsyncThread::start(Callback &&new_action) {
	action = std::move(new_action);
	start();
}

void AsyncThread::run_once() {
	if (action) {
		printf("Run once task id %u\n", internal->id);

		internal->thread = std::thread([this]() {
			action();
			internal->enabled = false;
			printf("Task %u: ran once and is done\n", internal->id);
		});
	}
}

void AsyncThread::stop() {
	if (action) {
		internal->enabled = false;

		printf("Sending stop signal to task id %u\n", internal->id);
	} else
		printf("Can't stop unknown task id %u\n", internal->id);
}

AsyncThread::~AsyncThread() {
	stop();

	if (internal->thread.joinable())
		internal->thread.join();
}

bool AsyncThread::is_enabled() {
	return internal->enabled && internal->thread.joinable();
}

void start_module_threads() {
	kill_signal = false;
	pause_signal = false;
}

void pause_module_threads() {
	pause_signal = true;
}

void kill_module_threads() {
	kill_signal = true;
}

} // namespace MetaModule
