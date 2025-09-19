#pragma once
#include <cstdint>
#include <cstring>
#include <map>
#include <string>

struct MappableObj {
	enum class Type { None, Knob, Button, InputJack, OutputJack, Toggle, MidiNote, MidiGate } objType;
	int64_t objID;
	int64_t moduleID;

	bool operator==(const MappableObj &rhs) const {
		return (objType == rhs.objType) && (objID == rhs.objID) && (moduleID == rhs.moduleID);
	}
};

namespace std
{
template<>
struct hash<MappableObj> {
	std::size_t operator()(const MappableObj &k) const {
		using std::hash;
		return ((hash<int>()(k.objID) ^ (hash<int64_t>()(k.moduleID) << 1)) >> 1) ^
			   (hash<int>()(static_cast<int>(k.objType)) << 1);
	}
};

} // namespace std
