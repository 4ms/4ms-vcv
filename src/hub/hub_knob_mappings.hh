#pragma once
#include "console/pr_dbg.hh"
#include "mapping/MappableObject.h"
#include "mapping/map_palette.hh"
#include "mapping/mapping.hh"

// Test: quitting normally could fail to save a valid map in the patch.json.
// 1) As VCV quits, it removes modules
// 2) A module with maps is deleted,
// 3) therefore VCV removes the ParamHandle.
// 4) Then hub::knob::draw() is called, which calls is_valid()
// 5) which will notice the paramHandle is gone, and thus delete the map
// 6) Then, hub::encodeJson is called and writes out json without that map

namespace MetaModule
{

enum class ShouldLock { No, Yes };

template<size_t MaxMapsPerPot, size_t MaxKnobSets = 8>
class HubKnobMappings {
	int64_t hubModuleId = -1;
	unsigned activeSetId = 0;
	size_t num_knobs;

public:
	struct KnobMappingSet {
		rack::ParamHandle paramHandle;
		std::array<Mapping, MaxKnobSets> maps;
	};

	using KnobMultiMap = std::array<KnobMappingSet, MaxMapsPerPot>;
	std::vector<KnobMultiMap> mappings;
	// Usage:
	// mappings[KnobId][MultiMapId].maps[KnobSetID].moduleId/paramId

	std::array<std::string, MaxKnobSets> knobSetNames;
	std::vector<std::array<std::string, MaxKnobSets>> aliases;

	HubKnobMappings(size_t num_knobs = 12)
		: num_knobs{num_knobs} {
		mappings.resize(num_knobs);
		aliases.resize(num_knobs);
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

	void changeActiveKnobSet(unsigned setId, ShouldLock do_lock) {
		if (setId == activeSetId || setId >= MaxKnobSets)
			return;

		updateMapsFromParamHandles();
		activeSetId = setId;
		refreshParamHandles(do_lock);
	}

	void setActiveKnobSetIdx(unsigned setId) {
		if (setId == activeSetId || setId >= MaxKnobSets)
			return;
		activeSetId = setId;
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

	// Curve Type
	uint8_t getCurveType(MappableObj paramObj) const {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				if (is_valid(mapset) && paramObj.moduleID == mapset.paramHandle.moduleId &&
					paramObj.objID == mapset.paramHandle.paramId)
				{
					return mapset.maps[activeSetId].curve_type;
				}
			}
		}

		return 0; //not found --> default value
	}

	void setCurveType(const MappableObj paramObj, uint8_t val) {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				if (is_valid(mapset) && paramObj.moduleID == mapset.paramHandle.moduleId &&
					paramObj.objID == mapset.paramHandle.paramId)
					mapset.maps[activeSetId].curve_type = val;
			}
		}
	}

	// Mapping Alias:
	void setMapAliasName(int64_t paramID, std::string const &newname, unsigned set_id) {
		if (paramID < (int)num_knobs) {
			aliases[paramID][set_id] = newname;
		}
	}

	void setMapAliasName(MappableObj paramObj, std::string const &newname, unsigned set_id) {
		setMapAliasName(paramObj.objID, newname, set_id);
	}

	void setMapAliasName(int64_t paramID, std::string const &newname) {
		setMapAliasName(paramID, newname, activeSetId);
	}

	void setMapAliasName(MappableObj paramObj, std::string const &newname) {
		setMapAliasName(paramObj.objID, newname, activeSetId);
	}

	std::string getMapAliasName(int64_t paramID, unsigned set_id) {
		if (paramID < (int)num_knobs) {
			return aliases[paramID][set_id];
		}
		return "";
	}

	std::string getMapAliasName(MappableObj paramObj, unsigned set_id) {
		return getMapAliasName(paramObj.objID, set_id);
	}

	std::string getMapAliasName(MappableObj paramObj) {
		return getMapAliasName(paramObj.objID, activeSetId);
	}

	// Add mappings to a knob in the active set:
	Mapping *addMap(unsigned hubParamId, int64_t destModuleId, int destParamId, unsigned set_id, ShouldLock do_lock) {
		if (set_id >= MaxKnobSets) {
			// Recover from error if set_is is out of range
			set_id = activeSetId;
		}

		auto &knob = nextFreeMap(hubParamId, set_id);

		if (set_id == activeSetId) {
			updateParamHandle(do_lock, &knob.paramHandle, destModuleId, destParamId, true);
		}

		auto *map = &knob.maps[set_id];
		map->module_id = destModuleId;
		map->param_id = destParamId;
		return map;
	}

	Mapping *addMap(unsigned hubParamId, int64_t destModuleId, int destParamId, ShouldLock do_lock) {
		return addMap(hubParamId, destModuleId, destParamId, activeSetId, do_lock);
	}

	// Return a reference to an array of KnobMappingSets of a knob
	auto &getAllMappings(int hubParamId) {
		if (hubParamId >= (int)num_knobs)
			return nullmap;

		return mappings[hubParamId];
	}

	unsigned getNumActiveMappings(int hubParamId) {
		unsigned num = 0;

		if (hubParamId >= (int)num_knobs)
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
					json_object_set_new(thisMapJ, "DstModID", json_integer(map.module_id));
					json_object_set_new(thisMapJ, "DstObjID", json_integer(map.param_id));
					json_object_set_new(thisMapJ, "SrcModID", json_integer(hubModuleId));
					json_object_set_new(thisMapJ, "SrcObjID", json_integer(hubParamId));
					json_object_set_new(thisMapJ, "RangeMin", json_real(map.range_min));
					json_object_set_new(thisMapJ, "RangeMax", json_real(map.range_max));
					json_object_set_new(thisMapJ, "CurveType", json_integer(map.curve_type));
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
			clear_all(ShouldLock::No);

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
							if (hubParamId >= (int)num_knobs)
								continue;

							val = json_object_get(mappingJ, "DstModID");
							auto destModuleId = json_is_integer(val) ? json_integer_value(val) : -1;

							val = json_object_get(mappingJ, "DstObjID");
							auto destModuleParamId = json_is_integer(val) ? json_integer_value(val) : -1;

							auto *map = addMap(hubParamId, destModuleId, destModuleParamId, set_i, ShouldLock::No);

							val = json_object_get(mappingJ, "RangeMin");
							map->range_min = json_is_real(val) ? json_real_value(val) : 0.f;

							val = json_object_get(mappingJ, "RangeMax");
							map->range_max = json_is_real(val) ? json_real_value(val) : 1.f;

							val = json_object_get(mappingJ, "CurveType");
							map->curve_type = json_is_integer(val) ? json_integer_value(val) : 0;

							val = json_object_get(mappingJ, "AliasName");
							std::string name = json_is_string(val) ? json_string_value(val) : "";
							setMapAliasName(hubParamId, name, set_i);
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
			if (mapset.maps[set_idx].module_id < 0) {
				return mapset;
			}
		}

		// If all are used, then overwrite the last one
		return mappings[hubParamId][MaxMapsPerPot - 1];
	}

	// checks if the mapset has a valid paramhandle in the active set
	bool is_valid(KnobMappingSet mapset) const {
		bool paramHandleValid = mapset.paramHandle.module && mapset.paramHandle.moduleId >= 0;
		if (!paramHandleValid) {
			mapset.maps[activeSetId].clear();
		}
		return paramHandleValid;
	}

	bool is_valid(Mapping map) {
		return map.module_id >= 0 && map.param_id >= 0;
	}

	void updateMapsFromParamHandles() {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				mapset.maps[activeSetId].module_id = mapset.paramHandle.moduleId;
				mapset.maps[activeSetId].param_id = mapset.paramHandle.paramId;
			}
		}
	}

	void updateParamHandle(
		ShouldLock do_lock, rack::ParamHandle *paramHandle, int64_t moduleId, int paramId, bool overwrite = true) {
		if (do_lock == ShouldLock::Yes)
			APP->engine->updateParamHandle(paramHandle, moduleId, paramId, overwrite);
		else
			APP->engine->updateParamHandle_NoLock(paramHandle, moduleId, paramId, overwrite);
	}

public:
	void refreshParamHandles(ShouldLock do_lock) {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				Mapping &map = mapset.maps[activeSetId];
				// Calling updateParamHandle(ph, m, p) where ph.moduleId == m && ph.paramId == p,
				// will remove paramHandle from the engine. That is, calling updateParamHandle()
				// without changing the module or param values will delete the paramHandle.
				// To fix this, rack::Engine would need to check if oldParamHandle == paramHandle
				if (mapset.paramHandle.moduleId != map.module_id || mapset.paramHandle.paramId != map.param_id) {
					updateParamHandle(do_lock, &mapset.paramHandle, map.module_id, map.param_id, true);
				}
			}
		}
	}

	void clear_all(ShouldLock do_lock) {
		// invalidate all maps
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				updateParamHandle(do_lock, &mapset.paramHandle, -1, 0, true);
				for (auto &map : mapset.maps) {
					map.clear();
				}
			}
		}
	}

	// Removes all maps that point to deleted modules
	void removeMapsToDeletedModules() {
		for (auto &knob : mappings) {
			for (auto &mapset : knob) {
				for (auto &map : mapset.maps) {
					if (map.module_id >= 0) {
						if (APP->engine->getModule(map.module_id) == nullptr) {
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

} // namespace MetaModule
