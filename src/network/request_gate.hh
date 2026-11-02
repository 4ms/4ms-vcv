#pragma once
#include <atomic>
#include <mutex>
#include <optional>

namespace MetaModule::network
{

// Lets one request to the Wi-Fi Expander run at a time.
// Foreground requests (sending a patch) have priority: background requests (status polling)
// are skipped while one is waiting or running, and should stop early when foreground_waiting().
class RequestGate {
public:
	using Lock = std::unique_lock<std::mutex>;

	// Blocks until no other request is running
	Lock foreground() {
		waiting++;
		Lock lock{mtx};
		waiting--;
		return lock;
	}

	// nullopt: skip this request, the gate is busy or a foreground request is waiting
	std::optional<Lock> try_background() {
		if (waiting > 0)
			return std::nullopt;

		Lock lock{mtx, std::try_to_lock};
		if (!lock.owns_lock())
			return std::nullopt;

		return lock;
	}

	bool foreground_waiting() const {
		return waiting > 0;
	}

private:
	std::mutex mtx;
	std::atomic<int> waiting{0};
};

} // namespace MetaModule::network
