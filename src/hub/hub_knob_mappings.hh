#pragma once
#include "mapping/MappableObject.h"
#include "mapping/map_palette.hh"
#include "mapping/mapping.hh"
#include "util/countzip.hh"
#include "util/static_string.hh"

// Test: quitting normally could fail to save a valid map in the patch.json.
// 1) As VCV quits, it removes modules
// 2) A module with maps is deleted,
// 3) therefore VCV removes the ParamHandle.
// 4) Then hub::knob::draw() is called, which calls is_valid()
// 5) which will notice the paramHandle is gone, and thus delete the map
// 6) Then, hub::encodeJson is called and writes out json without that map

//Rename HubKnobMapManager
template<size_t NumKnobs, size_t MaxMapsPerPot, size_t MaxKnobSets = 8>
class HubKnobMappings {
	int64_t hubModuleId = -1;
	unsigned activeSetId = 0;

public:
	struct KnobMappingSet {
		rack::ParamHandle paramHandle;
		std::array<Mapping, MaxKnobSets> maps;
	};

	using KnobMultiMap = std::array<KnobMappingSet, MaxMapsPerPot>;
	using HubKnobsMultiMaps = std::array<KnobMultiMap, NumKnobs>;
	HubKnobsMultiMaps mappings;

	std::array<std::string, MaxKnobSets> knobSetNames;
	// TODO aliases
	std::array<std::array<StaticString<31>, MaxKnobSets>, NumKnobs> aliases;

	HubKnobMappings() {
		for (unsigned i = 0; auto &knob_multimap : mappings) {
			auto color = PaletteHub::color(i++);
			for (auto &map : knob_multimap) {
				map.paramHandle.color = color;
				APP->engine->addParamHandle(&map.paramHandle);
				for (auto &m : map.maps)
					m.clear();
			}
		}
	}

	~HubKnobMappings() {
		for (auto &knob_multimap : mappings) {
			for (auto &map : knob_multimap)
				APP->engine->removeParamHandle(&map.paramHandle);
		}
	}

	void linkToModule(int64_t moduleId) {
		// Can't do this in the ctor because Rack sets the id after module is constructed
		hubModuleId = moduleId;
	}

	Mapping &activeMap(KnobMappingSet &mapset) {
		return mapset.maps[activeSetId];
	}

	// Sets of Knob Mappings:
	std::string_view getKnobSetName(unsigned idx) {
		if (idx < knobSetNames.size())
			return knobSetNames[idx];
		else
			return "";
	}

	std::string_view getActiveKnobSetName() {
		if (activeSetId < knobSetNames.size())
			return knobSetNames[activeSetId];
		else
			return "";
	}

	void setKnobSetName(unsigned idx, std::string const &name) {
		if (idx < knobSetNames.size())
			knobSetNames[idx] = name;
	}

	unsigned getActiveKnobSetIdx() {
		return activeSetId;
	}

	void setActiveKnobSetIdx(unsigned setId) {
		if (setId == activeSetId || setId >= MaxKnobSets)
			return;

		updateMapsFromParamHandles();
		activeSetId = setId;
		refreshParamHandles();
	}

	// Mapping Range:

	void setRangeMin(const MappableObj paramObj, float val) {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				if (is_valid(mapset) && paramObj.moduleID == mapset.paramHandle.moduleId &&
					paramObj.objID == mapset.paramHandle.paramId)
				{
					mapset.maps[activeSetId].range_min = val;
					return;
				}
			}
		}
	}

	void setRangeMax(const MappableObj paramObj, float val) {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				if (is_valid(mapset) && paramObj.moduleID == mapset.paramHandle.moduleId &&
					paramObj.objID == mapset.paramHandle.paramId)
					mapset.maps[activeSetId].range_max = val;
			}
		}
	}

	std::pair<float, float> getRange(const MappableObj paramObj) {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				if (is_valid(mapset) && paramObj.moduleID == mapset.paramHandle.moduleId &&
					paramObj.objID == mapset.paramHandle.paramId)
				{
					Mapping &map = mapset.maps[activeSetId];
					return {map.range_min, map.range_max};
				}
			}
		}
		return {0, 1}; //not found --> default value
	}

	// Mapping Alias:

	void setMapAliasName(MappableObj paramObj, std::string newname) {
		if (paramObj.objID < (int)NumKnobs) {
			aliases[paramObj.objID][activeSetId].copy(newname);
			auto &knobmultimap = mappings[paramObj.objID];
			auto &mapset = knobmultimap[0]; //first map per pot
			mapset.maps[activeSetId].alias_name = newname;
		}
	}

	std::string getMapAliasName(MappableObj paramObj) {
		if (paramObj.objID < (int)NumKnobs) {
			return std::string{aliases[paramObj.objID][activeSetId]};
		}
		return "";
	}

	// Add mappings to a knob in the active set:
	Mapping *addMap(unsigned hubParamId, int64_t destModuleId, int destParamId, unsigned set_id) {
		if (set_id >= MaxKnobSets) {
			// Recover from error if set_is is out of range
			set_id = activeSetId;
		}

		auto &knob = nextFreeMap(hubParamId, set_id);

		if (set_id == activeSetId) {
			APP->engine->updateParamHandle(&knob.paramHandle, destModuleId, destParamId, true);
		}

		auto *map = &knob.maps[set_id];
		map->moduleId = destModuleId;
		map->paramId = destParamId;
		return map;
	}

	Mapping *addMap(unsigned hubParamId, int64_t destModuleId, int destParamId) {
		return addMap(hubParamId, destModuleId, destParamId, activeSetId);
	}

	// Return a reference to an array of KnobMappingSets of a knob
	auto &getAllMappings(int hubParamId) {
		if (hubParamId >= (int)NumKnobs)
			return nullmap;

		return mappings[hubParamId];
	}

	unsigned getNumActiveMappings(int hubParamId) {
		unsigned num = 0;

		if (hubParamId >= (int)NumKnobs)
			return 0;

		for (auto &mapset : mappings[hubParamId]) {
			if (is_valid(mapset))
				num++;
		}
		return num;
	}

	// Save/restore VCV rack patch

	json_t *encodeJson() {
		json_t *rootJ = json_object();
		json_t *knobSetsJ = json_array();

		// Iterate mappings x MaxKnobSets times

		removeMapsToDeletedModules();
		updateMapsFromParamHandles();

		for (unsigned knobSetId = 0; knobSetId < MaxKnobSets; knobSetId++) {
			json_t *mapsJ = json_array();

			for (unsigned hubParamId = 0; auto &knob : mappings) {

				for (auto &mapsets : knob) {
					auto &map = mapsets.maps[knobSetId];

					if (!is_valid(map))
						continue;

					json_t *thisMapJ = json_object();
					json_object_set_new(thisMapJ, "DstModID", json_integer(map.moduleId));
					json_object_set_new(thisMapJ, "DstObjID", json_integer(map.paramId));
					json_object_set_new(thisMapJ, "SrcModID", json_integer(hubModuleId));
					json_object_set_new(thisMapJ, "SrcObjID", json_integer(hubParamId));
					json_object_set_new(thisMapJ, "RangeMin", json_real(map.range_min));
					json_object_set_new(thisMapJ, "RangeMax", json_real(map.range_max));
					json_object_set_new(thisMapJ, "AliasName", json_string(aliases[hubParamId][knobSetId].c_str()));

					json_array_append(mapsJ, thisMapJ);
					json_decref(thisMapJ);
				}
				hubParamId++;
			}
			json_array_append(knobSetsJ, mapsJ);
			json_decref(mapsJ);
		}
		json_object_set_new(rootJ, "Mappings", knobSetsJ);

		json_t *namesJ = json_array();
		for (auto &name : knobSetNames) {
			json_t *nameJ = json_string(name.c_str());
			json_array_append(namesJ, nameJ);
			json_decref(nameJ);
		}
		json_object_set_new(rootJ, "KnobSetNames", namesJ);

		return rootJ;
	}

	void decodeJson(json_t *rootJ) {
		auto knobSetsJ = json_object_get(rootJ, "Mappings");

		if (json_is_array(knobSetsJ)) {
			clear_all();

			for (size_t set_i = 0; set_i < json_array_size(knobSetsJ); set_i++) {
				auto mapsJ = json_array_get(knobSetsJ, set_i);

				if (json_is_array(mapsJ)) {

					for (size_t i = 0; i < json_array_size(mapsJ); i++) {
						auto mappingJ = json_array_get(mapsJ, i);

						if (json_is_object(mappingJ)) {
							json_t *val;

							//FIXME: at this point, VCV has not assigned an ID to this module, so we can't know if it matches this Hub's ID
							// Do we need to verify this json is for this hub (there may be more than one hub)
							// val = json_object_get(mappingJ, "SrcModID");
							// auto moduleId = json_is_integer(val) ? json_integer_value(val) : -1;
							// if (moduleId != hubModuleId)
							// 	continue;

							val = json_object_get(mappingJ, "SrcObjID");
							auto hubParamId = json_is_integer(val) ? json_integer_value(val) : -1;
							if (hubParamId >= (int)NumKnobs)
								continue;

							val = json_object_get(mappingJ, "DstModID");
							auto destModuleId = json_is_integer(val) ? json_integer_value(val) : -1;

							val = json_object_get(mappingJ, "DstObjID");
							auto destModuleParamId = json_is_integer(val) ? json_integer_value(val) : -1;

							auto *map = addMap(hubParamId, destModuleId, destModuleParamId, set_i);

							val = json_object_get(mappingJ, "RangeMin");
							map->range_min = json_is_real(val) ? json_real_value(val) : 0.f;

							val = json_object_get(mappingJ, "RangeMax");
							map->range_max = json_is_real(val) ? json_real_value(val) : 1.f;

							val = json_object_get(mappingJ, "AliasName");
							aliases[hubParamId][set_i] = json_is_string(val) ? json_string_value(val) : "";
						}
					}

					// refreshParamHandles();
				}
			}
		}

		auto namesJ = json_object_get(rootJ, "KnobSetNames");
		for (unsigned set_i = 0; auto &name : knobSetNames) {
			if (set_i < json_array_size(namesJ)) {
				auto nameJ = json_array_get(namesJ, set_i);
				name = json_is_string(nameJ) ? json_string_value(nameJ) : "";
			} else
				name = "";
			set_i++;
		}
	}

private:
	KnobMappingSet &nextFreeMap(unsigned hubParamId, unsigned set_idx) {
		// Find first unused mapping slot
		for (auto &mapset : mappings[hubParamId]) {
			if (mapset.maps[set_idx].moduleId < 0) {
				return mapset;
			}
		}

		// If all are used, then overwrite the last one
		return mappings[hubParamId][MaxMapsPerPot - 1];
	}

	// checks if the mapset has a valid paramhandle in the active set
	bool is_valid(KnobMappingSet mapset) {
		bool paramHandleValid = mapset.paramHandle.module && mapset.paramHandle.moduleId >= 0;
		if (!paramHandleValid) {
			mapset.maps[activeSetId].clear();
		}
		return paramHandleValid;
	}

	bool is_valid(Mapping map) {
		return map.moduleId >= 0 && map.paramId >= 0;
	}

	void updateMapsFromParamHandles() {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				mapset.maps[activeSetId].moduleId = mapset.paramHandle.moduleId;
				mapset.maps[activeSetId].paramId = mapset.paramHandle.paramId;
			}
		}
	}

	void refreshParamHandles() {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				Mapping &map = mapset.maps[activeSetId];
				APP->engine->updateParamHandle(&mapset.paramHandle, map.moduleId, map.paramId, true);
			}
		}
	}

	void clear_all() {
		// invalidate all maps
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				APP->engine->updateParamHandle(&mapset.paramHandle, -1, 0, true);
				for (auto &map : mapset.maps) {
					map.clear();
				}
			}
		}
	}

public:
	// Removes all maps that point to deleted modules
	void removeMapsToDeletedModules() {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				for (auto &map : mapset.maps) {
					if (map.moduleId >= 0) {
						if (APP->engine->getModule(map.moduleId) == nullptr) {
							map.clear();
						}
					}
				}
			}
		}
	}

	// Helpers

	auto begin() {
		return mappings.begin();
	}
	auto end() {
		return mappings.end();
	}

	KnobMultiMap nullmap{};
};
