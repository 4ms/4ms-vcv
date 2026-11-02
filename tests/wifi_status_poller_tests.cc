#include "doctest.h"
#include "flatbuffers/encode.hh"
#include "flat/all_generated.h"
#include "hub/wifi_status_poller.hh"
#include <atomic>
#include <chrono>
#include <mutex>
#include <string_view>
#include <thread>

using namespace MetaModule;
using namespace MetaModule::FlatBuffers;
using namespace std::chrono_literals;

namespace
{

std::vector<uint8_t> status_reply(uint8_t cpu_load, bool playing) {
	flatbuffers::FlatBufferBuilder fbb;
	auto status = CreateStatus(fbb, cpu_load, playing);
	auto message = CreateMessage(fbb, AnyMessage_Status, status.Union());
	fbb.Finish(message);
	auto data = fbb.GetBufferSpan();
	return {data.begin(), data.end()};
}

std::vector<uint8_t> timeout_reply() {
	std::string_view s = "Request timed out";
	return {s.begin(), s.end()};
}

// Fake transport: counts calls, remembers the last URL, replies with whatever is set
struct FakeBridge {
	std::atomic<int> calls{0};
	std::atomic<bool> overlapped{false};
	std::atomic<bool> in_flight{false};
	std::chrono::milliseconds delay{0};
	std::atomic<bool> skip{false}; // request skipped (e.g. a patch is being sent)

	std::mutex mtx;
	std::string last_url;
	std::vector<uint8_t> reply = status_reply(47, true);

	void set_reply(std::vector<uint8_t> r) {
		std::lock_guard lock{mtx};
		reply = std::move(r);
	}

	std::string url() {
		std::lock_guard lock{mtx};
		return last_url;
	}

	WifiStatusPoller::RequestFn fn() {
		return [this](std::string const &url, std::span<uint8_t>) {
			if (in_flight.exchange(true))
				overlapped = true;
			calls++;
			std::this_thread::sleep_for(delay);
			std::optional<std::vector<uint8_t>> r;
			{
				std::lock_guard lock{mtx};
				last_url = url;
				if (!skip)
					r = reply;
			}
			in_flight = false;
			return r;
		};
	}
};

template<typename F>
bool eventually(F cond, std::chrono::milliseconds timeout = 1000ms) {
	auto end = std::chrono::steady_clock::now() + timeout;
	while (std::chrono::steady_clock::now() < end) {
		if (cond())
			return true;
		std::this_thread::sleep_for(5ms);
	}
	return cond();
}

} // namespace

TEST_CASE("format_cpu_badge") {
	CHECK(format_cpu_badge(WifiStatus{47, true}) == "47%");
	CHECK(format_cpu_badge(WifiStatus{47, false}) == "—");
	CHECK(format_cpu_badge(std::nullopt) == "—");
}

TEST_CASE("reports latest status") {
	FakeBridge bridge;
	WifiStatusPoller poller{bridge.fn(), 20ms, 50ms};
	poller.set_url("http://mm");
	poller.set_active(true);

	CHECK(eventually([&] { return poller.latest() == WifiStatus{47, true}; }, 500ms));
}

TEST_CASE("failure clears status") {
	FakeBridge bridge;
	WifiStatusPoller poller{bridge.fn(), 20ms, 50ms};
	poller.set_url("http://mm");
	poller.set_active(true);
	REQUIRE(eventually([&] { return poller.latest().has_value(); }));

	bridge.set_reply(timeout_reply());
	CHECK(eventually([&] { return !poller.latest().has_value(); }));
}

TEST_CASE("skipped request keeps last status and retries soon") {
	FakeBridge bridge;
	WifiStatusPoller poller{bridge.fn(), 20ms, 5000ms};
	poller.set_url("http://mm");
	poller.set_active(true);
	REQUIRE(eventually([&] { return poller.latest().has_value(); }));

	bridge.skip = true;
	int calls_before = bridge.calls;
	// Retries at the normal interval, not the 5 s failure interval
	REQUIRE(eventually([&] { return bridge.calls > calls_before + 2; }, 500ms));
	CHECK(poller.latest() == WifiStatus{47, true});
}

TEST_CASE("asks for per-module loads when told to") {
	std::atomic<int> with_modules{0};
	std::atomic<int> without_modules{0};
	auto request = [&](std::string const &, std::span<uint8_t> body) -> std::optional<std::vector<uint8_t>> {
		auto status = GetMessage(body.data())->content_as_Status();
		(status && status->include_modules() ? with_modules : without_modules)++;
		return status_reply(10, true);
	};

	WifiStatusPoller poller{request, 10ms, 10ms};
	poller.set_url("http://mm");
	poller.set_active(true);
	REQUIRE(eventually([&] { return without_modules > 0; }));
	CHECK(with_modules == 0);

	poller.set_include_modules(true);
	CHECK(eventually([&] { return with_modules > 0; }));
}

TEST_CASE("MetaModule not answering is reported after a few bridge timeouts") {
	FakeBridge bridge;
	bridge.set_reply(timeout_reply());
	WifiStatusPoller poller{bridge.fn(), 5ms, 5ms};
	poller.set_url("http://mm");
	poller.set_active(true);

	CHECK(eventually([&] { return poller.metamodule_not_answering(); }));
	CHECK(bridge.calls >= 3);

	// An answer clears it
	bridge.set_reply(status_reply(10, true));
	CHECK(eventually([&] { return !poller.metamodule_not_answering(); }));
}

TEST_CASE("no reply from the bridge is not the MetaModule's fault") {
	FakeBridge bridge;
	bridge.set_reply({}); // e.g. Wi-Fi Expander unreachable
	WifiStatusPoller poller{bridge.fn(), 5ms, 5ms};
	poller.set_url("http://mm");
	poller.set_active(true);

	REQUIRE(eventually([&] { return bridge.calls > 5; }));
	CHECK_FALSE(poller.metamodule_not_answering());
}

TEST_CASE("inactive makes no requests") {
	FakeBridge bridge;
	WifiStatusPoller poller{bridge.fn(), 20ms, 50ms};
	poller.set_url("http://mm");

	std::this_thread::sleep_for(200ms);
	CHECK(bridge.calls == 0);

	poller.set_active(true);
	REQUIRE(eventually([&] { return bridge.calls > 0; }));
	poller.set_active(false);
	CHECK_FALSE(poller.latest().has_value());

	int calls_after_stop = bridge.calls;
	std::this_thread::sleep_for(100ms);
	CHECK(bridge.calls == calls_after_stop);

	for (int i = 0; i < 20; i++)
		poller.set_active(i % 2 == 0);
	poller.set_active(false);
}

TEST_CASE("stopping does not wait for a request in flight") {
	FakeBridge bridge;
	bridge.delay = 500ms;
	WifiStatusPoller poller{bridge.fn(), 20ms, 50ms};
	poller.set_url("http://mm");
	poller.set_active(true);
	REQUIRE(eventually([&] { return bridge.in_flight.load(); }));

	auto start = std::chrono::steady_clock::now();
	poller.set_active(false);
	CHECK(std::chrono::steady_clock::now() - start < 100ms);
	CHECK_FALSE(poller.latest().has_value());
}

TEST_CASE("shutdown stops polling for good") {
	FakeBridge bridge;
	WifiStatusPoller poller{bridge.fn(), 20ms, 50ms};
	poller.set_url("http://mm");
	poller.set_active(true);
	REQUIRE(eventually([&] { return bridge.calls > 0; }));

	poller.shutdown();
	int calls_after_shutdown = bridge.calls;

	poller.set_active(true);
	std::this_thread::sleep_for(100ms);
	CHECK(bridge.calls == calls_after_shutdown);
	CHECK_FALSE(poller.latest().has_value());
}

TEST_CASE("no requests without a url") {
	FakeBridge bridge;
	WifiStatusPoller poller{bridge.fn(), 20ms, 50ms};
	poller.set_active(true);

	std::this_thread::sleep_for(100ms);
	CHECK(bridge.calls == 0);
}

TEST_CASE("url change applies to next request") {
	FakeBridge bridge;
	WifiStatusPoller poller{bridge.fn(), 20ms, 50ms};
	poller.set_url("http://a");
	poller.set_active(true);
	REQUIRE(eventually([&] { return bridge.url() == "http://a"; }));

	poller.set_url("http://b");
	CHECK(eventually([&] { return bridge.url() == "http://b"; }));
}

TEST_CASE("never overlaps own requests") {
	FakeBridge bridge;
	bridge.delay = 30ms;
	WifiStatusPoller poller{bridge.fn(), 1ms, 1ms};
	poller.set_url("http://mm");
	poller.set_active(true);

	std::this_thread::sleep_for(300ms);
	poller.set_active(false);
	CHECK(bridge.calls > 1);
	CHECK_FALSE(bridge.overlapped);
}
