#pragma once
#include <cstdint>
#include <cstring>
#include <map>
#include <string>

struct MappableObj {
	enum class Type { None, Knob, InputJack, OutputJack, Toggle, MidiNote, MidiGate } objType;
	int64_t objID;
	int64_t moduleID;

	bool operator==(const MappableObj &rhs) const {
		return (objType == rhs.objType) && (objID == rhs.objID) && (moduleID == rhs.moduleID);
	}

	bool mappable_to(const Type otherType) const {
		return (objType == otherType) || (objType == Type::Knob && otherType == Type::MidiNote) ||
			   (objType == Type::MidiNote && otherType == Type::Knob) ||
			   (objType == Type::MidiGate && otherType == Type::Knob) ||
			   (objType == Type::Knob && otherType == Type::MidiGate);
	}

	const char *objTypeStr() const {
		if (objType == Type::Knob)
			return "Knob";
		if (objType == Type::InputJack)
			return "InputJack";
		if (objType == Type::OutputJack)
			return "OutputJack";
		if (objType == Type::Toggle)
			return "Toggle";
		if (objType == Type::MidiNote)
			return "MidiValue";
		if (objType == Type::MidiGate)
			return "MidiGate";
		return "None";
	}

	void setObjTypeFromString(const char *str) {
		if (std::strcmp(str, "Knob") == 0)
			objType = Type::Knob;
		else if (std::strcmp(str, "InputJack") == 0)
			objType = Type::InputJack;
		else if (std::strcmp(str, "OutputJack") == 0)
			objType = Type::OutputJack;
		else if (std::strcmp(str, "Toggle") == 0)
			objType = Type::Toggle;
		else if (std::strcmp(str, "MidiValue") == 0)
			objType = Type::MidiNote;
		else if (std::strcmp(str, "MidiGate") == 0)
			objType = Type::MidiGate;
		else
			objType = Type::None;
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
