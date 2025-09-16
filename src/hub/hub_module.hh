#pragma once
#include "comm/comm_module.hh"
#include "hub/hub_module.hh"
#include "hub/jack_alias.hh"
#include "hub_knob_mappings.hh"
#include "mapping/module_directory.hh"
#include "plugin.hh"
#include "util/edge_detector.hh"
#include "util/math.hh"
#include "util/string_util.hh"
#include <osdialog.h>
#include <span>

namespace MetaModule
{

struct MetaModuleHubBase : public rack::Module {

	MetaModuleHubBase(unsigned num_pots)
		: mappings{num_pots}
		, last_knob_val(num_pots) {
	}

	std::function<void()> updatePatchName;
	std::string patchNameText = "";
	std::string patchDescText = "";
	MappingMode mappingMode = MetaModule::MappingMode::ALL;

	bool should_save = false;
	bool should_send_wifi = false;

	std::optional<int> inProgressMapParamId{};

	static constexpr uint32_t MaxNumPots = 12;
	static constexpr uint32_t MaxMapsPerPot = 8;
	static constexpr uint32_t MaxKnobSets = 8;
	HubKnobMappings<MaxMapsPerPot, MaxKnobSets> mappings;

	JackAlias jack_alias{};

	std::vector<float> last_knob_val{};

	// Mapping State/Progress

	void startMappingFrom(int hubParamId) {
		inProgressMapParamId = hubParamId;
	}

	void setMappingMode(int index) {
		mappingMode = MappingMode(index);
	}

	void endMapping() {
		inProgressMapParamId = {};
	}

	std::optional<int> getMappingSource() {
		return inProgressMapParamId;
	}

	bool isMappingInProgress() {
		return inProgressMapParamId.has_value();
	}

	// Always locks
	bool registerMap(int hubParamId, rack::Module *module, int64_t moduleParamId) {
		if (!isMappingInProgress()) {
			pr_dbg("registerMap() called but we aren't mapping\n");
			return false;
		}

		if (!module) {
			pr_dbg("Error: Dest module ptr is null. Aborting mapping.\n");
			endMapping();
			return false;
		}

		if (ModuleDirectory::isHub(module)) {
			pr_dbg("Dest module is a hub. Aborting mapping.\n");
			endMapping();
			return false;
		}

		mappings.linkToModule(id);
		auto *map = mappings.addMap(hubParamId, module->id, moduleParamId, ShouldLock::Yes);
		if (!map) {
			pr_dbg("Error: could not create mapping\n");
			return false;
		}

		map->range_max = 1.f;
		map->range_min = 0.0f;
		map->curve_type = 0;
		endMapping();

		return true;
	}

	// Runtime applying maps

	void processMaps() {
		for (int hubParamId = 0; auto &knob : mappings) {
			if (std::fabs(last_knob_val[hubParamId] - params[hubParamId].getValue()) > 0.0001f) {
				auto new_val = params[hubParamId].getValue();
				auto last_val = last_knob_val[hubParamId];

				for (auto &mapset : knob) {

					int paramId = mapset.paramHandle.paramId;
					auto module = mapset.paramHandle.module;
					if (!module)
						continue;

					rack::ParamQuantity *paramQuantity = module->paramQuantities[paramId];
					if (!paramQuantity)
						continue;
					if (!paramQuantity->isBounded())
						continue;

					auto &map = mappings.activeMap(mapset);

					if (map.curve_type == 1) {
						// Toggle/latching mode:

						if (new_val > 0.5f && last_val < 0.5f) {
							// if param is currently closer to min, then set it to max (and vice-versa)
							auto cur_val = paramQuantity->getScaledValue();
							if (std::abs(cur_val - map.range_min) < std::abs(cur_val - map.range_max)) {
								paramQuantity->setScaledValue(map.range_max);
							} else {
								paramQuantity->setScaledValue(map.range_min);
							}
						}

					} else {
						auto val = MathTools::map_value(new_val, 0.f, 1.f, map.range_min, map.range_max);
						paramQuantity->setScaledValue(val);
					}
				}

				last_knob_val[hubParamId] = new_val;
			}
			hubParamId++;
		}
	}

	// VCV Rack calls this periodically on auto-save
	json_t *dataToJson() override {
		json_t *rootJ = mappings.encodeJson();

		json_t *aliasJ = jack_alias.encodeJson();
		json_object_set_new(rootJ, "Alias", aliasJ);

		if (updatePatchName) {
			updatePatchName();
			json_t *patchNameJ = json_string(patchNameText.c_str());
			json_object_set_new(rootJ, "PatchName", patchNameJ);

			json_t *patchDescJ = json_string(patchDescText.c_str());
			json_object_set_new(rootJ, "PatchDesc", patchDescJ);

			json_t *defaultKnobSetJ = json_integer(mappings.getActiveKnobSetIdx());
			json_object_set_new(rootJ, "DefaultKnobSet", defaultKnobSetJ);

			json_t *mappingModeJ = json_integer(this->mappingMode);
			json_object_set_new(rootJ, "MappingMode", mappingModeJ);
		} else {
			pr_err("Error: Widget has not been constructed, but dataToJson is being called\n");
		}
		return rootJ;
	}

	// VCV Rack calls this on startup, and on loading a new patch file
	void dataFromJson(json_t *rootJ) override {
		auto patchNameJ = json_object_get(rootJ, "PatchName");
		if (json_is_string(patchNameJ)) {
			patchNameText = json_string_value(patchNameJ);
		}

		auto patchDescJ = json_object_get(rootJ, "PatchDesc");
		if (json_is_string(patchDescJ)) {
			patchDescText = json_string_value(patchDescJ);
		}

		auto defaultKnobSetJ = json_object_get(rootJ, "DefaultKnobSet");
		if (json_is_integer(defaultKnobSetJ)) {
			unsigned idx = json_integer_value(defaultKnobSetJ);
			mappings.changeActiveKnobSet(idx, ShouldLock::No);
		}

		auto mappingModeJ = json_object_get(rootJ, "MappingMode");
		if (json_is_integer(mappingModeJ)) {
			mappingMode = MappingMode(json_integer_value(mappingModeJ));
		}

		auto aliasJ = json_object_get(rootJ, "Alias");
		jack_alias.decodeJson(aliasJ);

		mappings.decodeJson(rootJ);
	}

	void onReset(const ResetEvent &e) override {
		Module::onReset(e);
		patchNameText = "";
		patchDescText = "";
		mappingMode = MetaModule::MappingMode::ALL;
		mappings.clear_all(ShouldLock::No);
		mappings.setActiveKnobSetIdx(0);
		mappings.refreshParamHandles(ShouldLock::No);
	}

	enum class JackDir { In, Out };

	void set_jack_alias(JackDir dir, unsigned idx, std::string_view text) {
		if (dir == JackDir::In) {
			if (idx < jack_alias.in.size()) {
				jack_alias.in[idx] = text;
				inputInfos[idx]->name = text;
			}
		} else {
			if (idx < jack_alias.out.size()) {
				jack_alias.out[idx] = text;
				outputInfos[idx]->name = text;
			}
		}
	}

	std::string_view get_jack_alias(JackDir dir, unsigned idx) {
		if (dir == JackDir::In) {
			if (idx < jack_alias.in.size())
				return jack_alias.in[idx];

		} else if (dir == JackDir::Out) {
			if (idx < jack_alias.out.size())
				return jack_alias.out[idx];
		}

		return "";
	}
};

} // namespace MetaModule
