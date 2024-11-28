#include "CoreModules/async_thread.hh"
#include <thread>

namespace MetaModule
{

struct AsyncThread::Internal {
	std::atomic<bool> enabled = false;
	std::thread thread;
};

namespace //anonymous
{

std::atomic<bool> kill_signal = false;
std::atomic<bool> pause_signal = false;

} // namespace

AsyncThread::AsyncThread()
	: internal{new AsyncThread::Internal} {
}

AsyncThread::AsyncThread(Callback &&new_action)
	: action(std::move(new_action))
	, internal{new AsyncThread::Internal} {
}

AsyncThread::~AsyncThread() {
	internal->enabled = false;
	internal->thread.join();
}

void AsyncThread::start(unsigned module_id) {
	if (!action) {
		internal->enabled = false;
		return;
	}

	internal->enabled = true;

	internal->thread = std::thread([this]() {
		while (internal->enabled && !kill_signal) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			if (!pause_signal)
				action();
		}
	});
}

void AsyncThread::start(unsigned module_id, Callback &&new_action) {
	action = std::move(new_action);
	start(module_id);
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
