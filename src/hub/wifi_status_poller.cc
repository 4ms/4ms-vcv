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
	{
		std::lock_guard lock{mtx};
		shutting_down = true;
	}
	cv.notify_all();

	if (thread.joinable())
		thread.join();
}

void WifiStatusPoller::set_active(bool is_active) {
	{
		std::lock_guard lock{mtx};
		active = is_active;

		if (!active)
			status.reset();

		// The thread is started once and sleeps while inactive,
		// so stopping never waits for a request in flight
		if (active && !thread.joinable())
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
		lock.unlock();

		bool skipped = false;
		std::optional<FlatBuffers::WifiStatus> result;
		try {
			auto body = FlatBuffers::encode_status_request();
			if (auto reply = request(request_url, body))
				result = FlatBuffers::decode_status_response(*reply);
			else
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
		cv.wait_for(lock, result ? interval_ok : interval_fail, [this] { return shutting_down || !active; });
	}
}

std::string format_cpu_badge(std::optional<FlatBuffers::WifiStatus> status) {
	if (!status || !status->playing)
		return "—";

	return std::to_string(status->cpu_load) + "%";
}

} // namespace MetaModule
