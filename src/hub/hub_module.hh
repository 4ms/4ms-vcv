#pragma once
#include "comm/comm_module.hh"
#include "hub/hub_module.hh"
#include "hub_knob_mappings.hh"
#include "mapping/module_directory.hh"
#include "mapping/vcv_patch_file_writer.hh"
#include "plugin.hh"
#include "util/edge_detector.hh"
#include "util/math.hh"
#include "util/string_util.hh"
#include <osdialog.h>
#include <span>

namespace MetaModule
{

struct MetaModuleHubBase : public rack::Module {

	std::function<void()> updatePatchName;
	std::string patchNameText = "";
	std::string patchDescText = "";
	std::string wifiUrl = MetaModule::wifiUrl;
	std::string wifiPath = MetaModule::wifiVolume == MetaModule::Volume::Card	  ? "Card" :
						   MetaModule::wifiVolume == MetaModule::Volume::USB	  ? "USB" :
						   MetaModule::wifiVolume == MetaModule::Volume::Internal ? "Internal" :
																					"Card";

	std::optional<int> inProgressMapParamId{};

	// FIXME: NumPots should be a template parameter
	// We then need a common base class widgets can point to
	static constexpr uint32_t NumPots = 12;
	static constexpr uint32_t MaxMapsPerPot = 8;
	static constexpr uint32_t MaxKnobSets = 8;
	HubKnobMappings<NumPots, MaxMapsPerPot, MaxKnobSets> mappings;

	std::array<float, NumPots> last_knob_val{};

	// Mapping State/Progress

	void startMappingFrom(int hubParamId) {
		inProgressMapParamId = hubParamId;
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
		endMapping();

		return true;
	}

	// Runtime applying maps

	void processMaps() {
		for (int hubParamId = 0; auto &knob : mappings) {
			if (std::fabs(last_knob_val[hubParamId] - params[hubParamId].getValue()) > 0.0001f) {
				last_knob_val[hubParamId] = params[hubParamId].getValue();

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
					auto val = MathTools::map_value(last_knob_val[hubParamId], 0.f, 1.f, map.range_min, map.range_max);
					paramQuantity->setScaledValue(val);
				}
			}
			hubParamId++;
		}
	}

	// VCV Rack calls this periodically on auto-save
	json_t *dataToJson() override {
		json_t *rootJ = mappings.encodeJson();

		if (updatePatchName) {
			updatePatchName();
			json_t *patchNameJ = json_string(patchNameText.c_str());
			json_object_set_new(rootJ, "PatchName", patchNameJ);

			json_t *patchDescJ = json_string(patchDescText.c_str());
			json_object_set_new(rootJ, "PatchDesc", patchDescJ);

			json_t *defaultKnobSetJ = json_integer(mappings.getActiveKnobSetIdx());
			json_object_set_new(rootJ, "DefaultKnobSet", defaultKnobSetJ);

			json_t *wifiUrlJ = json_string(wifiUrl.c_str());
			json_object_set_new(rootJ, "WifiURL", wifiUrlJ);

			json_t *wifiPathJ = json_string(wifiPath.c_str());
			json_object_set_new(rootJ, "WifiPath", wifiPathJ);
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

		auto wifiUrlJ = json_object_get(rootJ, "WifiURL");
		if (json_is_string(wifiUrlJ)) {
			wifiUrl = json_string_value(wifiUrlJ);
		}

		auto wifiPathJ = json_object_get(rootJ, "WifiPath");
		if (json_is_string(wifiPathJ)) {
			wifiPath = json_string_value(wifiPathJ);
		}

		mappings.decodeJson(rootJ);
	}

	void onReset(const ResetEvent &e) override {
		Module::onReset(e);
		patchNameText = "";
		patchDescText = "";
		mappings.clear_all(ShouldLock::No);
		mappings.setActiveKnobSetIdx(0);
		mappings.refreshParamHandles(ShouldLock::No);
	}
};

} // namespace MetaModule
