#include "encode.hh"
#include "flat/all_generated.h"
#include <string_view>
#include <vector>

namespace MetaModule::FlatBuffers
{

std::vector<uint8_t> encode_file(std::string_view filename,
								 std::string_view contents,
								 std::string_view volume,
								 bool load_after_save) {
	flatbuffers::FlatBufferBuilder fbb;

	auto contentsF = fbb.CreateVector((uint8_t *)contents.data(), contents.size());
	auto volumeF = fbb.CreateString(volume);
	auto filenameF = fbb.CreateString(filename);
	auto upload_patch = CreateUploadPatch(fbb, contentsF, volumeF, filenameF, load_after_save);

	auto message = CreateMessage(fbb, AnyMessage_UploadPatch, upload_patch.Union());
	fbb.Finish(message);

	// copy data
	auto data = fbb.GetBufferSpan();
	std::vector<uint8_t> bytes;
	bytes.insert(bytes.begin(), data.begin(), data.end());
	return bytes;
}

std::pair<bool, std::string> decode_response(std::vector<uint8_t> rawmsg) {
	auto verifier = flatbuffers::Verifier(rawmsg.data(), rawmsg.size());
	auto ok = VerifyMessageBuffer(verifier);
	if (ok) {
		if (rawmsg.size() > 6) {
			auto msg = GetMessage(rawmsg.data());

			if (auto result = msg->content_as_Result()) {
				auto result_msg = flatbuffers::GetStringView(result->message());
				return {result->success(), std::string(result_msg)};
			}
		}
	} else {
		std::string_view strmsg = std::string_view{(const char *)rawmsg.data(), rawmsg.size()};

		if (strmsg == "Request timed out") {
			return {false, "Timed out"};
		}
	}

	return {false, "No response"};
}

bool is_bridge_timeout(std::span<const uint8_t> rawmsg) {
	return std::string_view{(const char *)rawmsg.data(), rawmsg.size()} == "Request timed out";
}

bool decode_load_requested(std::span<const uint8_t> rawmsg) {
	auto verifier = flatbuffers::Verifier(rawmsg.data(), rawmsg.size());
	if (!VerifyMessageBuffer(verifier))
		return false;

	auto result = GetMessage(rawmsg.data())->content_as_Result();
	return result && result->load_requested();
}

std::vector<uint8_t> encode_status_request(bool include_modules) {
	flatbuffers::FlatBufferBuilder fbb;

	auto status = CreateStatus(fbb, 0, false, include_modules);
	auto message = CreateMessage(fbb, AnyMessage_Status, status.Union());
	fbb.Finish(message);

	auto data = fbb.GetBufferSpan();
	return {data.begin(), data.end()};
}

std::optional<WifiStatus> decode_status_response(std::span<const uint8_t> rawmsg) {
	auto verifier = flatbuffers::Verifier(rawmsg.data(), rawmsg.size());
	if (!VerifyMessageBuffer(verifier))
		return std::nullopt;

	auto status = GetMessage(rawmsg.data())->content_as_Status();
	if (!status)
		return std::nullopt;

	WifiStatus result{status->cpu_load(), status->playing()};

	if (auto name = status->patch_name())
		result.patch_name = name->str();
	if (auto loads = status->module_loads())
		result.module_loads.assign(loads->begin(), loads->end());
	result.overhead = status->overhead();
	result.module_loads_valid = status->module_loads_valid();
	if (auto cores = status->module_cores())
		result.module_cores.assign(cores->begin(), cores->end());
	if (auto core_loads = status->core_loads())
		result.core_loads.assign(core_loads->begin(), core_loads->end());
	result.patch_fingerprint = status->patch_fingerprint();

	return result;
}

}; // namespace MetaModule::FlatBuffers
