#include "wifi_status_poller.hh"

namespace MetaModule
{

WifiStatusPoller::WifiStatusPoller(RequestFn request,
								   std::chrono::milliseconds interval_ok,
								   std::chrono::milliseconds interval_fail)
	: request{std::move(request)}
	, interval_ok{interval_ok}
	, interval_fail{interval_fail} {
}

WifiStatusPoller::~WifiStatusPoller() {
	shutdown();
}

void WifiStatusPoller::shutdown() {
	{
		std::lock_guard lock{mtx};
		shutting_down = true;
		status.reset();
	}
	cv.notify_all();

	if (thread.joinable())
		thread.join();
}

void WifiStatusPoller::set_active(bool is_active) {
	{
		std::lock_guard lock{mtx};
		active = is_active;

		if (!active) {
			status.reset();
			bridge_timeouts = 0;
		}

		// The thread is started once and sleeps while inactive,
		// so stopping never waits for a request in flight
		if (active && !thread.joinable() && !shutting_down)
			thread = std::thread{[this] { run(); }};
	}
	cv.notify_all();
}

void WifiStatusPoller::set_url(std::string new_url) {
	{
		std::lock_guard lock{mtx};
		url = std::move(new_url);
	}
	cv.notify_all();
}

void WifiStatusPoller::set_include_modules(bool include) {
	std::lock_guard lock{mtx};
	include_modules = include;
}

bool WifiStatusPoller::metamodule_not_answering() const {
	std::lock_guard lock{mtx};
	return bridge_timeouts >= BridgeTimeoutsBeforeReporting;
}

std::optional<FlatBuffers::WifiStatus> WifiStatusPoller::latest() const {
	std::lock_guard lock{mtx};
	return status;
}

void WifiStatusPoller::run() {
	std::unique_lock lock{mtx};

	while (!shutting_down) {
		if (!active || url.empty()) {
			status.reset();
			cv.wait(lock, [this] { return shutting_down || (active && !url.empty()); });
			continue;
		}

		auto request_url = url;
		auto request_modules = include_modules;
		lock.unlock();

		bool skipped = false;
		bool bridge_timeout = false;
		std::optional<FlatBuffers::WifiStatus> result;
		try {
			auto body = FlatBuffers::encode_status_request(request_modules);
			if (auto reply = request(request_url, body)) {
				result = FlatBuffers::decode_status_response(*reply);
				bridge_timeout = FlatBuffers::is_bridge_timeout(*reply);
				if (result)
					result->modules_requested = request_modules;
			} else
				skipped = true;
		} catch (...) {
			// Treat any failure as no answer: never let an exception escape this thread
		}

		lock.lock();

		// Stopped while the request was in flight: drop the result
		if (shutting_down || !active)
			continue;

		// Skipped: keep showing the last status and try again soon
		if (skipped) {
			cv.wait_for(lock, interval_ok, [this] { return shutting_down || !active; });
			continue;
		}

		status = result;
		if (result)
			bridge_timeouts = 0;
		else if (bridge_timeout)
			bridge_timeouts++;
		cv.wait_for(lock, result ? interval_ok : interval_fail, [this] { return shutting_down || !active; });
	}
}

std::string format_cpu_badge(std::optional<FlatBuffers::WifiStatus> status) {
	if (!status || !status->playing)
		return "—";

	return std::to_string(status->cpu_load) + "%";
}

} // namespace MetaModule
