#pragma once
#include "flatbuffers/encode.hh"
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <thread>
#include <vector>

namespace MetaModule
{

// Polls the MetaModule for its status over Wi-Fi in a background thread.
// The Hub reads the latest result from the GUI thread.
class WifiStatusPoller {
public:
	// Returns the reply, or nullopt if the request was skipped (e.g. a patch is being sent)
	using RequestFn =
		std::function<std::optional<std::vector<uint8_t>>(std::string const &url, std::span<uint8_t> body)>;

	WifiStatusPoller(RequestFn request,
					 std::chrono::milliseconds interval_ok = std::chrono::milliseconds{1000},
					 std::chrono::milliseconds interval_fail = std::chrono::milliseconds{5000});
	~WifiStatusPoller();

	// Starts or stops polling
	void set_active(bool active);

	// Address of the Wi-Fi Expander, used from the next request on. Empty: don't poll.
	void set_url(std::string url);

	// Also ask for per-module loads (makes the MetaModule measure them)
	void set_include_modules(bool include);

	// Stops polling for good and waits for the thread to finish.
	// Call before the plugin is unloaded (joining in a static destructor can deadlock on Windows).
	void shutdown();

	// nullopt: no answer (yet)
	std::optional<FlatBuffers::WifiStatus> latest() const;

	// Shared by all Hubs, polls the Wi-Fi Expander over HTTP
	static WifiStatusPoller &instance();

private:
	void run();

	RequestFn request;
	std::chrono::milliseconds interval_ok;
	std::chrono::milliseconds interval_fail;

	mutable std::mutex mtx;
	std::condition_variable cv;
	std::string url;
	std::optional<FlatBuffers::WifiStatus> status;
	bool active = false;
	bool include_modules = false;
	bool shutting_down = false;

	std::thread thread;
};

// Text for the Hub's CPU badge: "47%", or "—" if there's nothing to show
std::string format_cpu_badge(std::optional<FlatBuffers::WifiStatus> status);

} // namespace MetaModule
