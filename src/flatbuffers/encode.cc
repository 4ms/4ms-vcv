#include "flat/all_generated.h"
#include <string_view>
#include <vector>

namespace MetaModule::FlatBuffers
{

std::vector<uint8_t> encode_file(std::string_view filename, std::string_view contents, std::string_view volume) {
	flatbuffers::FlatBufferBuilder fbb;

	auto contentsF = fbb.CreateVector((uint8_t *)contents.data(), contents.size());
	auto volumeF = fbb.CreateString(volume);
	auto filenameF = fbb.CreateString(filename);
	auto upload_patch = CreateUploadPatch(fbb, contentsF, volumeF, filenameF);

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

}; // namespace MetaModule::FlatBuffers
