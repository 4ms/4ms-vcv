#include "doctest.h"
#include "flatbuffers/encode.hh"
#include "flat/all_generated.h"
#include <string_view>

using namespace MetaModule::FlatBuffers;

namespace
{

std::vector<uint8_t> make_status_message(uint8_t cpu_load, bool playing) {
	flatbuffers::FlatBufferBuilder fbb;
	auto status = CreateStatus(fbb, cpu_load, playing);
	auto message = CreateMessage(fbb, AnyMessage_Status, status.Union());
	fbb.Finish(message);
	auto data = fbb.GetBufferSpan();
	return {data.begin(), data.end()};
}

std::vector<uint8_t> make_result_message() {
	flatbuffers::FlatBufferBuilder fbb;
	auto result = CreateResult(fbb, true);
	auto message = CreateMessage(fbb, AnyMessage_Result, result.Union());
	fbb.Finish(message);
	auto data = fbb.GetBufferSpan();
	return {data.begin(), data.end()};
}

} // namespace

TEST_CASE("status request is a Status message") {
	auto request = encode_status_request();

	auto verifier = flatbuffers::Verifier(request.data(), request.size());
	REQUIRE(VerifyMessageBuffer(verifier));
	CHECK(GetMessage(request.data())->content_type() == AnyMessage_Status);
}

TEST_CASE("decodes Status response") {
	auto msg = make_status_message(47, true);
	CHECK(decode_status_response(msg) == WifiStatus{47, true});

	msg = make_status_message(0, false);
	CHECK(decode_status_response(msg) == WifiStatus{0, false});
}

TEST_CASE("status request asks for per-module loads only when told to") {
	auto plain = encode_status_request();
	CHECK_FALSE(GetMessage(plain.data())->content_as_Status()->include_modules());

	auto with_modules = encode_status_request(true);
	CHECK(GetMessage(with_modules.data())->content_as_Status()->include_modules());
}

TEST_CASE("decodes per-module loads") {
	flatbuffers::FlatBufferBuilder fbb;
	auto name = fbb.CreateString("My Patch");
	std::vector<uint16_t> loads{0, 32, 105};
	auto loads_vec = fbb.CreateVector(loads);
	auto status = CreateStatus(fbb, 40, true, false, name, loads_vec, 41, true);
	fbb.Finish(CreateMessage(fbb, AnyMessage_Status, status.Union()));
	auto span = fbb.GetBufferSpan();
	std::vector<uint8_t> msg{span.begin(), span.end()};

	auto decoded = decode_status_response(msg);
	REQUIRE(decoded.has_value());
	CHECK(decoded->cpu_load == 40);
	CHECK(decoded->playing);
	CHECK(decoded->patch_name == "My Patch");
	CHECK(decoded->module_loads == loads);
	CHECK(decoded->overhead == 41);
	CHECK(decoded->module_loads_valid);
}

TEST_CASE("decodes module cores and core loads") {
	flatbuffers::FlatBufferBuilder fbb;
	std::vector<uint8_t> cores{0, 0, 1};
	std::vector<uint16_t> core_loads{200, 197};
	auto cores_vec = fbb.CreateVector(cores);
	auto core_loads_vec = fbb.CreateVector(core_loads);
	auto status = CreateStatus(fbb, 35, true, false, 0, 0, 150, true, cores_vec, core_loads_vec);
	fbb.Finish(CreateMessage(fbb, AnyMessage_Status, status.Union()));
	auto span = fbb.GetBufferSpan();
	std::vector<uint8_t> msg{span.begin(), span.end()};

	auto decoded = decode_status_response(msg);
	REQUIRE(decoded.has_value());
	CHECK(decoded->module_cores == cores);
	CHECK(decoded->core_loads == core_loads);
}

TEST_CASE("decodes patch fingerprint") {
	flatbuffers::FlatBufferBuilder fbb;
	auto status = CreateStatus(fbb, 40, true, false, 0, 0, 0, true, 0, 0, 89770027u);
	fbb.Finish(CreateMessage(fbb, AnyMessage_Status, status.Union()));
	auto span = fbb.GetBufferSpan();
	std::vector<uint8_t> msg{span.begin(), span.end()};

	auto decoded = decode_status_response(msg);
	REQUIRE(decoded.has_value());
	CHECK(decoded->patch_fingerprint == 89770027u);
}

TEST_CASE("reply without per-module fields decodes with empty loads") {
	auto decoded = decode_status_response(make_status_message(47, true));
	REQUIRE(decoded.has_value());
	CHECK(decoded->patch_name.empty());
	CHECK(decoded->module_loads.empty());
	CHECK(decoded->overhead == 0);
	CHECK_FALSE(decoded->module_loads_valid);
}

TEST_CASE("rejects non-Status and garbage") {
	std::string_view timeout = "Request timed out";
	std::vector<uint8_t> timeout_bytes(timeout.begin(), timeout.end());
	CHECK_FALSE(decode_status_response(timeout_bytes).has_value());

	std::vector<uint8_t> empty;
	CHECK_FALSE(decode_status_response(empty).has_value());

	CHECK_FALSE(decode_status_response(make_result_message()).has_value());

	std::vector<uint8_t> junk{0x12, 0x34, 0x56};
	CHECK_FALSE(decode_status_response(junk).has_value());
}
