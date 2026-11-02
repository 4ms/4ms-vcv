#include "doctest.h"
#include "network/request_gate.hh"
#include <atomic>
#include <chrono>
#include <thread>

using namespace MetaModule::network;
using namespace std::chrono_literals;

TEST_CASE("background request runs when the gate is idle") {
	RequestGate gate;
	auto lock = gate.try_background();
	CHECK(lock.has_value());
}

TEST_CASE("background request is skipped while another request runs") {
	RequestGate gate;
	auto fg = gate.foreground();
	CHECK_FALSE(gate.try_background().has_value());
}

TEST_CASE("foreground request waits for a background request and asks it to stop") {
	RequestGate gate;
	auto bg = gate.try_background();
	REQUIRE(bg.has_value());
	CHECK_FALSE(gate.foreground_waiting());

	std::atomic<bool> acquired = false;
	std::thread fg_thread{[&] {
		auto fg = gate.foreground();
		acquired = true;
	}};

	auto end = std::chrono::steady_clock::now() + 1s;
	while (!gate.foreground_waiting() && std::chrono::steady_clock::now() < end)
		std::this_thread::sleep_for(1ms);
	CHECK(gate.foreground_waiting());
	CHECK_FALSE(acquired);

	// A new background request must not jump ahead of the waiting foreground one
	bg.reset();
	fg_thread.join();
	CHECK(acquired);
	CHECK_FALSE(gate.foreground_waiting());
}
