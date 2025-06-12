#include "CoreModules/async_thread.hh"
#include <thread>

namespace MetaModule
{

struct AsyncThread::Internal {
	std::atomic<bool> enabled = false;
	std::thread thread;
	bool one_shot = false;
};

namespace //anonymous
{

std::atomic<bool> kill_signal = false;
std::atomic<bool> pause_signal = false;

} // namespace

AsyncThread::AsyncThread(CoreProcessor *module)
	: internal{new AsyncThread::Internal} {
}

AsyncThread::AsyncThread(CoreProcessor *module, Callback &&new_action)
	: action(std::move(new_action))
	, internal{new AsyncThread::Internal} {
}

AsyncThread::~AsyncThread() {
	internal->enabled = false;
	if (internal->thread.joinable())
		internal->thread.join();
}

void AsyncThread::start() {
	if (!action) {
		internal->enabled = false;
		return;
	}

	internal->enabled = true;

	internal->thread = std::thread([this]() {
		while (internal->enabled && !kill_signal) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			if (!pause_signal) {
				action();
				if (internal->one_shot) {
					internal->enabled = false;
				}
			}
		}
	});
}

void AsyncThread::start(Callback &&new_action) {
	action = std::move(new_action);
	start();
}

void AsyncThread::stop() {
	internal->enabled = false;
	if (internal->thread.joinable())
		internal->thread.join();
}

void AsyncThread::run_once() {
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
