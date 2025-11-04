#include "async_thread_control.hh"
#include "console/pr_dbg.hh"
#include "util/fixed_vector.hh"
#include <atomic>
#include <cstdlib>
#include <optional>
#include <thread>

namespace MetaModule::Async
{

namespace
{

static FixedVector<Async::Task, 64> tasks;

std::atomic<bool> is_running = false;

std::atomic<bool> kill_signal = false;

struct SafeWrapper {
	// Rack does not call plugin destroy() so we need to
	// use RAII to do cleanup:

	std::thread async_task_runner;

	~SafeWrapper() {
		kill_module_threads();
	}
} runner;

std::optional<uint32_t> task_index(uint32_t id) {
	if (auto found = std::ranges::find(tasks, id, &Async::Task::id); found != tasks.end()) {
		return std::distance(tasks.begin(), found);
	} else {
		pr_warn("Cannot find task with id %u\n", (unsigned)id);
		return std::nullopt;
	}
}

} // namespace

std::optional<uint32_t> create_task(CoreProcessor *module) {
	if (auto id = tasks.push_back_for_overwrite(); id != tasks.max_size()) {
		tasks[id].enabled = false;
		tasks[id].id = std::rand();
		return tasks[id].id;
	} else {
		pr_err("Maximum async tasks already created (%u) -- cannot add more\n", unsigned(tasks.max_size()));
		return std::nullopt;
	}
}

void destroy_task(uint32_t id) {
	if (auto idx = task_index(id)) {
		pr_trace("Erase task id %u (index %u)\n", (unsigned)id, (unsigned)(*idx));
		tasks.erase(*idx);
	}
}

Async::Task *get_task(unsigned id) {
	if (auto idx = task_index(id))
		return &tasks[*idx];
	else
		return nullptr;
}

void start_module_threads() {
	bool expected = false;
	if (is_running.compare_exchange_strong(expected, true)) {
		pr_trace("Starting module async thread\n");

		runner.async_task_runner = std::thread([=]() {
			while (true) {
				if (kill_signal) {
					pr_trace("Got kill signal\n");
					return;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1));

				for (auto &task : tasks) {
					if (task.enabled /*&& !pause_signal*/) {
						task.action();

						if (task.one_shot)
							task.enabled = false;
					}
				}
			}
			pr_trace("Module async thread ending\n");
		});
	}
}

void kill_module_threads() {
	if (is_running) {
		pr_trace("Killing threads...\n");

		kill_signal = true;

		auto start = std::chrono::steady_clock::now().time_since_epoch().count() / 1'000'000LL;

		while (true) {
			if (runner.async_task_runner.joinable()) {
				runner.async_task_runner.join();
				return;
			}

			auto now = std::chrono::steady_clock::now().time_since_epoch().count() / 1'000'000LL;
			if (now - start > 3000) {
				return; //thread crashed, so we just crash on exit?!
			}
		}

		is_running = false;
	}
}

} // namespace MetaModule::Async
