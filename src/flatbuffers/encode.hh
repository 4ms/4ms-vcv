#include <string>

namespace MetaModule::FlatBuffers
{
std::vector<uint8_t> encode_file(std::string_view filename, std::string_view contents, std::string_view volume);
}
