#pragma once
#include "patch/patch.hh"
#include <array>
#include <rack.hpp>

namespace MetaModule::MIDI
{

struct ModuleIds {
	int64_t midiCV = -1;
	int64_t midiMaps = -1;
	int64_t midiGate = -1;
	int64_t midiCC = -1;
};

struct MidiCVSettings {
	float pitchwheelRange = 1;
	unsigned channels = 0;
	unsigned midi_chan = 0;
	unsigned clockDivJack = 0;
	PolyMode polyMode = PolyMode::Rotate;

	int64_t voctSplitModuleId = -1;
	int64_t gateSplitModuleId = -1;
	int64_t velSplitModuleId = -1;
	int64_t aftSplitModuleId = -1;
	int64_t retrigSplitModuleId = -1;
};

enum CoreMidiJacks {
	VoctJack = 0,
	GateJack = 1,
	VelJack = 2,
	AftJack = 3,
	PWJack,
	MWJack,
	RetrigJack = 6,
	ClockJack,
	ClockDivJack,
	StartJack,
	StopJack,
	ContJack
};

struct MidiGateSettings {
	std::array<int8_t, 128> notes{};
	bool velocity_mode = false;
	bool mpe_mode = false;
	unsigned midi_chan = 0;
};

struct MidiCCCVSettings {
	std::array<int8_t, 128> CCnums{};
	std::array<int8_t, 128> values{};
	bool smooth = false;
	bool mpe_mode = false;
	bool lsb_mode = false;
	unsigned midi_chan = 0;
};

struct CCKnobMap {
	int8_t CCnum;
	uint16_t param_id;
	int64_t module_id;
};

struct MidiCCKnobSettings {
	std::array<CCKnobMap, 128> ccs{};
	bool smooth = false;
	uint8_t midi_chan = 0;
};

struct Settings {
	MidiCVSettings CV;
	MidiGateSettings gate;
	MidiCCCVSettings CCCV;
	MidiCCKnobSettings CCKnob;
};

struct Modules {
	Settings settings;
	ModuleIds moduleIds;

	void addMidiModule(rack::Module *module);
	void addPolySplitCable(rack::Cable *cable);
	bool isPolySplitModule(rack::Module *module);
};

} // namespace MetaModule::MIDI
