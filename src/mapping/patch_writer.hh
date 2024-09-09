#pragma once
#include "JackMap.hh"
#include "ModuleID.h"
#include "ParamMap.hh"
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
	void addModuleStateJson(rack::Module *module);

	void addKnobMaps(unsigned panelKnobId, unsigned knobSetId, const std::span<const Mapping> maps);
	void addKnobMapSet(unsigned knobSetId, std::string_view knobSetName);

	std::string printPatchYAML();

	PatchData &get_data();
	static std::map<int64_t, uint16_t> squash_ids(std::vector<int64_t> ids);

private:
	void mapInputJack(const CableMap &map);
	void mapOutputJack(const CableMap &map);
	void mapMidiCVJack(CableMap &map);
	void mapMidiGateJack(CableMap &map);
	void mapMidiCCJack(CableMap &cable);

	void mapMidiCVPolySplitJack(CableMap &cable, unsigned monoJackId);

	void setModuleList(std::vector<BrandModule> &modules);

	PatchData pd;
	int64_t hubModuleId = -1;

	MIDI::ModuleIds midiModuleIds;
	MIDI::Settings midiSettings;

	std::map<int64_t, uint16_t> idMap; // idMap[64 bit VCV module id] -> 16 bit MM-patch module id
};

} // namespace MetaModule
