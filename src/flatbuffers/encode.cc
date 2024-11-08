#include "flat/all_generated.h"
#include <string_view>
#include <vector>

namespace MetaModule::FlatBuffers
{

std::vector<uint8_t> encode_file(std::string_view filename, std::string_view contents, std::string_view volume) {
	flatbuffers::FlatBufferBuilder fbb;

	// Convert contents to a vector of u8
	// std::vector<uint8_t> contents_u8;
	// contents_u8.insert(contents_u8.begin(), (uint8_t *)contents.begin(), (uint8_t *)contents.end());
	// auto upload_patch = CreateUploadPatchDirect(fbb, &contents_u8, volume.data(), filename.data());

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

}; // namespace MetaModule::FlatBuffers
