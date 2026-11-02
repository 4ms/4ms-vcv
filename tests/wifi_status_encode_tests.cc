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
