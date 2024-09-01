#pragma once
#include "comm/comm_module.hh"
#include "hub_knob_mappings.hh"
#include "mapping/module_directory.hh"
#include "mapping/vcv_patch_file_writer.hh"
#include "util/edge_detector.hh"
#include "util/math.hh"
#include "util/string_util.hh"
#include <osdialog.h>
#include <span>

// #define pr_dbg printf
#define pr_dbg(x, ...)

struct MetaModuleHubBase : public rack::Module {

	std::function<void()> updatePatchName;
	std::function<void()> updateDisplay;
	std::string labelText = "";
	std::string patchNameText = "";
	std::string patchDescText = "";
	std::string lastPatchFilePath = "";

	EdgeStateDetector patchWriteButton;
	bool ready_to_write_patch = false;

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
		auto *map = mappings.addMap(hubParamId, module->id, moduleParamId);
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

	// Handle writing patches

	void processPatchButton(float patchButtonState) {
		patchWriteButton.update(patchButtonState > 0.5f);
		if (patchWriteButton.went_high()) {
			updatePatchName();
			updateDisplay();
			ready_to_write_patch = true;
			// writePatchFile();
		}
	}

	bool should_write_patch() {
		auto t = ready_to_write_patch;
		ready_to_write_patch = false;
		return t;
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
		} else
			printf("Error: Widget has not been constructed, but dataToJson is being called\n");
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
			mappings.setActiveKnobSetIdx(idx);
		}

		mappings.decodeJson(rootJ);
	}

	void writePatchFile() {
		std::string patchName;
		std::string patchDir;

		if (patchNameText != "" && patchNameText != "Enter Patch Name") {
			patchName = patchNameText.c_str();
		} else {
			std::string randomname = "Unnamed" + std::to_string(MathTools::randomNumber<unsigned int>(10, 99));
			patchName = randomname.c_str();
		}

		ReplaceString patchStructName{patchName};
		patchStructName.replace_all(" ", "")
			.replace_all("-", "_")
			.replace_all(",", "_")
			.replace_all("/", "")
			.replace_all("\\", "")
			.replace_all("\"", "")
			.replace_all("'", "")
			.replace_all(".", "_")
			.replace_all("?", "")
			.replace_all("#", "")
			.replace_all("!", "");

		osdialog_filters *filters = osdialog_filters_parse("Metamodule Patch File (.yml):yml");
		DEFER({ osdialog_filters_free(filters); });

		std::string dir = lastPatchFilePath;
		if (dir == "")
			dir = rack::asset::userDir.c_str();

		char *filename = osdialog_file(OSDIALOG_SAVE, dir.c_str(), patchStructName.str.c_str(), filters);
		if (!filename)
			return;

		std::string patchFileName = filename;
		DEFER({ free(filename); });

		if (rack::system::getExtension(rack::system::getFilename(patchFileName)) != ".yml") {
			patchFileName += ".yml";
		}

		lastPatchFilePath = rack::system::getDirectory(patchFileName);

		labelText = "Creating patch...";
		updateDisplay();

		VCVPatchFileWriter<NumPots, MaxMapsPerPot, MaxKnobSets>::writePatchFile(
			id, mappings.mappings, mappings.knobSetNames, patchFileName, patchName, patchDescText);

		labelText = "Wrote patch file: ";
		labelText += rack::system::getFilename(patchFileName);
		updateDisplay();
	}
};
