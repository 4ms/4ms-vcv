#include "doctest.h"
#include "hub/cpu_monitor_setting.hh"
#include <jansson.h>

using namespace MetaModule;

namespace
{

CpuMonitor read(char const *json) {
	json_error_t err;
	auto *root = json_loads(json, 0, &err);
	REQUIRE(root);
	auto mode = read_cpu_monitor_setting(root);
	json_decref(root);
	return mode;
}

} // namespace

TEST_CASE("reads the CPU monitor setting") {
	CHECK(read(R"({"wifiCpuMonitor": 0})") == CpuMonitor::Off);
	CHECK(read(R"({"wifiCpuMonitor": 1})") == CpuMonitor::Total);
}

TEST_CASE("missing or invalid CPU monitor setting is off") {
	CHECK(read(R"({})") == CpuMonitor::Off);
	CHECK(read(R"({"wifiCpuMonitor": 7})") == CpuMonitor::Off);
	CHECK(read(R"({"wifiCpuMonitor": "x"})") == CpuMonitor::Off);
}

TEST_CASE("writes the CPU monitor setting") {
	auto *root = json_object();
	write_cpu_monitor_setting(root, CpuMonitor::Total);
	CHECK(json_integer_value(json_object_get(root, "wifiCpuMonitor")) == 1);
	CHECK(read_cpu_monitor_setting(root) == CpuMonitor::Total);
	json_decref(root);
}
