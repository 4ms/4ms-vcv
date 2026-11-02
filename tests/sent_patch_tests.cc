#include "doctest.h"
#include "hub/sent_patch.hh"

using namespace MetaModule;
using FlatBuffers::WifiStatus;

namespace
{

SentPatch sent_patch() {
	SentPatch sent;
	sent.id_map = {{100, 0}, {200, 1}, {300, 2}}; // 100 is the Hub
	sent.rack_module_ids = {100, 200, 300, 400}; // 400 isn't in the patch (e.g. unsupported)
	sent.fingerprint = 1234;
	return sent;
}

WifiStatus playing_status() {
	WifiStatus status{};
	status.cpu_load = 40;
	status.playing = true;
	status.patch_name = "My Patch";
	status.module_loads = {0, 32, 105};
	status.overhead = 41;
	status.module_loads_valid = true;
	status.patch_fingerprint = 1234;
	return status;
}

} // namespace

TEST_CASE("nothing sent yet is not synced") {
	CHECK(sync_state(std::nullopt, {100, 200}, playing_status()) == SyncState::NotSynced);
}

TEST_CASE("no reply or not playing means no data") {
	auto sent = sent_patch();
	CHECK(sync_state(sent, sent.rack_module_ids, std::nullopt) == SyncState::NoData);

	auto status = playing_status();
	status.playing = false;
	CHECK(sync_state(sent, sent.rack_module_ids, status) == SyncState::NoData);

	status = playing_status();
	status.module_loads_valid = false;
	CHECK(sync_state(sent, sent.rack_module_ids, status) == SyncState::NoData);
}

TEST_CASE("same patch and same modules is synced") {
	auto sent = sent_patch();
	CHECK(sync_state(sent, {400, 300, 200, 100}, playing_status()) == SyncState::Synced);
}

TEST_CASE("added or removed module is not synced") {
	auto sent = sent_patch();
	CHECK(sync_state(sent, {100, 200, 300, 400, 500}, playing_status()) == SyncState::NotSynced);
	CHECK(sync_state(sent, {100, 200, 400}, playing_status()) == SyncState::NotSynced);
}

TEST_CASE("different modules playing on the MetaModule is not synced, even with the same name") {
	auto sent = sent_patch();
	auto status = playing_status();
	status.patch_fingerprint = 5678;
	CHECK(sync_state(sent, sent.rack_module_ids, status) == SyncState::NotSynced);
}

TEST_CASE("reply without a fingerprint is not synced") {
	auto sent = sent_patch();
	auto status = playing_status();
	status.patch_fingerprint = 0;
	CHECK(sync_state(sent, sent.rack_module_ids, status) == SyncState::NotSynced);
}

TEST_CASE("the fingerprint decides, not the name") {
	auto sent = sent_patch();
	auto status = playing_status();
	status.patch_name = "Renamed on the MetaModule";
	CHECK(sync_state(sent, sent.rack_module_ids, status) == SyncState::Synced);
}

TEST_CASE("module load lookup") {
	auto sent = sent_patch();
	auto status = playing_status();

	CHECK(module_load_for(sent, status, 200) == 32);
	CHECK(module_load_for(sent, status, 300) == 105);
	CHECK_FALSE(module_load_for(sent, status, 400).has_value()); // not in patch
	CHECK_FALSE(module_load_for(sent, status, 999).has_value()); // unknown

	status.module_loads = {0, 32}; // shorter list than the patch
	CHECK_FALSE(module_load_for(sent, status, 300).has_value());
}

TEST_CASE("module core lookup") {
	auto sent = sent_patch();
	auto status = playing_status();
	status.module_cores = {0, 0, 1};

	CHECK(module_core_for(sent, status, 200) == 0);
	CHECK(module_core_for(sent, status, 300) == 1);
	CHECK_FALSE(module_core_for(sent, status, 400).has_value());

	status.module_cores = {}; // patch has no load balancing info
	CHECK_FALSE(module_core_for(sent, status, 300).has_value());
}

TEST_CASE("module label shows the core") {
	CHECK(format_module_label(195, 0) == "19.5% C1");
	CHECK(format_module_label(195, 1) == "19.5% C2");
	CHECK(format_module_label(195, std::nullopt) == "19.5%");
	CHECK(format_module_label(std::nullopt, 1) == "—");
}

TEST_CASE("hub label shows both cores and the overhead") {
	auto status = playing_status();
	status.core_loads = {200, 197};
	status.overhead = 150;
	CHECK(format_hub_loads(status) == "Core 1 20.0%  Core 2 19.7%  Overhead 15.0%");

	status.core_loads = {}; // older firmware
	CHECK(format_hub_loads(status) == "Overhead 15.0%");
}

TEST_CASE("format_module_load") {
	CHECK(format_module_load(32) == "3.2%");
	CHECK(format_module_load(0) == "0.0%");
	CHECK(format_module_load(1005) == "100.5%");
	CHECK(format_module_load(std::nullopt) == "—");
}
