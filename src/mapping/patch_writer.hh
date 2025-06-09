#pragma once
#include "JackMap.hh"
#include "ModuleID.h"
#include "ParamMap.hh"
#include "mapping/expanders.hh"
#include "mapping/mapping.hh"
#include "mapping/midi_modules.hh"
#include "patch/patch_data.hh"
#include <map>
#include <rack.hpp>
#include <span>
#include <vector>

namespace MetaModule
{

class PatchFileWriter {
public:
	PatchFileWriter(std::vector<BrandModule> modules, int64_t hubModuleId);
	~PatchFileWriter();

	void setPatchName(std::string patchName);
	void setPatchDesc(std::string patchDesc);
	void setCableList(std::vector<CableMap> &cables);
	void setParamList(std::vector<ParamMap> &params);
	void setMidiSettings(MIDI::ModuleIds &ids, MIDI::Settings const &settings);
	void setExpanders(ExpanderMappings const &exp);
	void addModuleStateJson(rack::Module *module);

	void addKnobMaps(unsigned panelKnobId, unsigned knobSetId, const std::span<const Mapping> maps);
	void addKnobMapSet(unsigned knobSetId, std::string_view knobSetName);

	std::string printPatchYAML();

	PatchData &get_data();
	static std::map<int64_t, uint16_t> squash_ids(std::vector<int64_t> ids);

private:
	void mapInputJack(CableMap &map);
	void mapOutputJack(CableMap &map);
	void mapMidiCVJack(CableMap &map, uint32_t midi_chan);
	void mapMidiGateJack(CableMap &map, unsigned midi_chan);
	void mapMidiCCJack(CableMap &cable, unsigned midi_chan);

	void mapMidiCVPolySplitJack(CableMap &cable, unsigned monoJackId, unsigned midi_chan);

	void setModuleList(std::vector<BrandModule> &modules);

	PatchData pd;
	int64_t hubModuleId = -1;

	MIDI::ModuleIds midiModuleIds;
	MIDI::Settings midiSettings;

	ExpanderMappings expanders;

	std::map<int64_t, uint16_t> idMap; // idMap[64 bit VCV module id] -> 16 bit MM-patch module id
};

} // namespace MetaModule
