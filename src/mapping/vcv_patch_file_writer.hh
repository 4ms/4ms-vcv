#pragma once
#include "cable_color.hh"
#include "console/pr_dbg.hh"
#include "cpputil/util/colors.hh"
#include "expanders/button_expander.hh"
#include "hub/hub_knob_mappings.hh"
#include "hub/hub_module.hh"
#include "hub/jack_alias.hh"
#include "mapping/JackMap.hh"
#include "mapping/ModuleID.h"
#include "mapping/ParamMap.hh"
#include "mapping/expanders.hh"
#include "mapping/midi_modules.hh"
#include "mapping/module_directory.hh"
#include "mapping/module_specific_fixes.hh"
#include "mapping/patch_writer.hh"
#include "plugin.hh"
#include <fstream>
#include <rack.hpp>

namespace MetaModule
{

// Adapts VCVRack-format of patch data to a format PatchFileWriter can use
template<size_t MaxMapsPerPot, size_t MaxKnobSets>
struct VCVPatchFileWriter {
	struct FileFields {
		int64_t hubModuleId;
		HubKnobMappings<MaxMapsPerPot, MaxKnobSets> &mappings;
		JackAlias &jack_aliases;
		std::string &patchName;
		std::string &patchDesc;
		MappingMode mappingMode;
		unsigned suggested_samplerate;
		unsigned suggested_blocksize;
		bool use_glue_labels = true;
		const std::map<int64_t, std::string> &module_aliases;
	};

	static std::string createPatchYml(FileFields data) {
		auto hubModuleId = data.hubModuleId;
		auto &mappings = data.mappings;
		auto &jack_aliases = data.jack_aliases;
		auto &patchName = data.patchName;
		auto &patchDesc = data.patchDesc;
		auto mappingMode = data.mappingMode;
		auto suggested_samplerate = data.suggested_samplerate;
		auto suggested_blocksize = data.suggested_blocksize;
		auto use_glue_labels = data.use_glue_labels;
		auto &module_aliases = data.module_aliases;

		auto context = rack::contextGet();
		auto engine = context->engine;

		// Find all compatible modules in this patch
		// TODO: only add modules that are mapped to this hub
		std::vector<BrandModule> moduleData;
		std::vector<ParamMap> paramData;
		std::vector<int64_t> splitModuleIds;
		MIDI::Modules midimodules;
		ExpanderMappings expanders;

		// First, find the hub module and add it to the lists
		auto *hubModule = engine->getModule(hubModuleId);
		if (ModuleDirectory::isHub(hubModule)) {
			addHubModuleToMapping(hubModule, moduleData);
		}

		if (mappingMode == MappingMode::ALL) {
			auto moduleIDs = engine->getModuleIds();
			for (auto moduleID : moduleIDs) {

				auto *module = engine->getModule(moduleID);

				addModuleToMapping(module, moduleData, paramData, midimodules, expanders);
			}
		}

		if (mappingMode == MappingMode::CONNECTED) {
			std::set<int64_t> connected_modules;
			connected_modules.insert(hubModuleId);

			auto cables = engine->getCableIds();

			unsigned num_found = 0;

			// iterate all cables until we find no more new modules
			while (connected_modules.size() != num_found) {

				num_found = connected_modules.size();

				for (auto cable_id : cables) {
					auto *cable = engine->getCable(cable_id);
					if (cable->inputModule && cable->outputModule) {
						auto input_id = cable->inputModule->getId();
						auto output_id = cable->outputModule->getId();

						if (connected_modules.contains(input_id)) {
							connected_modules.insert(output_id);
						} else if (connected_modules.contains(output_id)) {
							connected_modules.insert(input_id);
						}
					}
				}
			}

			for (auto module_id : connected_modules) {
				if (auto *module = engine->getModule(module_id))
					addModuleToMapping(module, moduleData, paramData, midimodules, expanders);
			}
		}

		if (mappingMode == MappingMode::LEFTRIGHT || mappingMode == MappingMode::LEFT) {

			// Add modules joined to the left of the hub module
			auto *module = engine->getModule(hubModuleId);
			while (module) {
				addModuleToMapping(module, moduleData, paramData, midimodules, expanders);

				if (module->leftExpander.moduleId < 0)
					break;

				module = module->leftExpander.module;
			}
		}

		if (mappingMode == MappingMode::LEFTRIGHT || mappingMode == MappingMode::RIGHT) {

			// Add modules joined to the right of the hub module
			auto *module = engine->getModule(hubModuleId);
			while (module) {
				addModuleToMapping(module, moduleData, paramData, midimodules, expanders);

				if (module->rightExpander.moduleId < 0)
					break;

				module = module->rightExpander.module;
			}
		}

		// Get all cables
		auto cable_ids = engine->getCableIds();
		std::vector<rack::engine::Cable *> cables;
		cables.reserve(cable_ids.size());
		for (auto id : cable_ids) {
			cables.push_back(engine->getCable(id));
		}

		// Scan cables for MIDICV -> Split connections
		for (auto cable : cables) {
			midimodules.addPolySplitCable(cable);
		}

		// Scan cables
		std::vector<CableMap> cableData;
		for (auto cableWidget : APP->scene->rack->getCompleteCables()) {
			auto cable = cableWidget->cable;

			auto out = cable->outputModule;
			auto in = cable->inputModule;

			// The output module must be in the patch, or a Core MIDI module, or a Split module connected to a Core MIDI module.
			bool isKnownOutModule = ModuleDirectory::isRegularModule(out) || ModuleDirectory::isHubOrExpander(out) ||
									ModuleDirectory::isCoreMIDI(out) || midimodules.isPolySplitModule(out);

			// The input module must be in the patch
			bool isKnownInputModule = ModuleDirectory::isRegularModule(in) || ModuleDirectory::isHubOrExpander(in);

			if (isKnownOutModule || isKnownInputModule) {

				// Ignore cables that are connected to a different hub or unknown expanders
				if (ModuleDirectory::isHub(out) && (out->getId() != hubModuleId))
					continue;
				if (ModuleDirectory::isHub(in) && (in->getId() != hubModuleId))
					continue;
				if (expanders.isUnknownExpanderCable(cable))
					continue;

				if (!apply_module_specific_fixes(cable, cableData)) {
					cableData.push_back({
						.outputJackId = cable->outputId,
						.inputJackId = cable->inputId,
						.outputModuleId = out->getId(),
						.inputModuleId = in->getId(),
						.lv_color_full = cable_color_rgb565(cableWidget),
					});
				}
			}
		}

		PatchFileWriter pw{moduleData, hubModuleId};
		pw.setMidiSettings(midimodules.settings);
		pw.setPatchName(patchName);
		pw.setPatchDesc(patchDesc);
		pw.setExpanders(expanders);
		pw.setCableList(cableData);
		pw.setParamList(paramData);
		pw.setSuggestedSamplerateBlocksize(suggested_samplerate, suggested_blocksize);

		// Read GLUE module labels as module aliases.
		// If multiple GLUE labels target the same module, pick the one with the highest
		// vertical position (lowest y coordinate) on the rack.
		if (use_glue_labels) {
			std::map<int64_t, std::pair<std::string, float>> moduleAliases; // {text, y}
			for (auto moduleID : engine->getModuleIds()) {
				auto *module = engine->getModule(moduleID);
				if (isGlueModule(module)) {
					if (auto *jsonData = module->dataToJson()) {
						if (auto *labels = json_object_get(jsonData, "labels")) {
							size_t i;
							json_t *label;
							json_array_foreach(labels, i, label) {
								auto *textVal = json_object_get(label, "text");
								auto *idVal = json_object_get(label, "moduleId");
								auto *yVal = json_object_get(label, "y");
								if (textVal && idVal && json_is_string(textVal) && json_is_integer(idVal)
								&& json_string_value(textVal)[0] != '\0') {
									int64_t vcv_mod_id = json_integer_value(idVal);
									float y = yVal ? (float)json_number_value(yVal) : 0.f;
									auto it = moduleAliases.find(vcv_mod_id);
									if (it == moduleAliases.end() || y < it->second.second)
										moduleAliases[vcv_mod_id] = {json_string_value(textVal), y};
								}
							}
						}
						json_decref(jsonData);
					}
				}
			}
			for (auto const &[vcv_id, text_y] : moduleAliases)
				pw.setModuleAlias(vcv_id, text_y.first);
		}

		// Manual aliases take precedence over GLUE labels
		for (auto const &[vcv_id, alias] : module_aliases)
			pw.setModuleAlias(vcv_id, alias);

		//combine hub aliases and expander aliases
		JackAlias aliases{jack_aliases};
		if (expanders.hasAudioExpander()) {
			auto hub_base = dynamic_cast<MetaModuleHubBase *>(engine->getModule(expanders.getAudioExpanderId()));
			if (hub_base) {
				auto &e = hub_base->jack_alias;
				aliases.in.insert(aliases.in.end(), e.in.begin(), e.in.end());
				aliases.out.insert(aliases.out.end(), e.out.begin(), e.out.end());
			}
		}
		pw.setJackAliases(aliases);

		// Add module state from Module::dataToJson()
		// Add bypass state from Module::isBypassed()
		for (auto moduleID : engine->getModuleIds()) {
			auto *module = engine->getModule(moduleID);
			if (ModuleDirectory::isRegularModule(module)) {
				pw.addModuleStateJson(module);
				pw.addBypassedModule(module);
			}
		}

		// Set knob set names
		for (unsigned set_i = 0; set_i < MaxKnobSets; set_i++) {
			pw.addKnobMapSet(set_i, mappings.knobSetNames[set_i]);
		}

		// Go through all hub knob mappings
		const auto firstPanelKnob = 0u;
		addAllMappings(firstPanelKnob, pw, mappings);

		// Go through all button expander mappings
		auto button_exps = expanders.getButtonExpanderIds();
		for (auto moduleID : button_exps) {
			auto *module = engine->getModule(moduleID);
			if (auto buttonExp = dynamic_cast<ButtonExpanderModule *>(module)) {
				const auto firstButtonParamId = FirstButton + (buttonExp->buttonExpanderId * ButtonsPerExpander);
				addAllMappings(firstButtonParamId, pw, buttonExp->mappings);
			}
		}

		return pw.printPatchYAML();
	}

	static void addAllMappings(unsigned startingPanelId,
							   PatchFileWriter &pw,
							   HubKnobMappings<MaxMapsPerPot, MaxKnobSets> &mappings) {

		mappings.updateMapsFromParamHandles();

		for (unsigned set_i = 0; set_i < MaxKnobSets; set_i++) {
			for (unsigned panelId = startingPanelId; auto &knob_maps : mappings.mappings) {

				std::vector<Mapping> active_maps;
				active_maps.reserve(MaxMapsPerPot);

				for (auto &mapsets : knob_maps) {
					auto &map = mapsets.maps[set_i];
					map.alias_name = mappings.getMapAliasName(panelId - startingPanelId, set_i);
					if (map.module_id > 0)
						active_maps.push_back(map);
				}

				if (active_maps.size())
					pw.addKnobMaps(panelId, set_i, active_maps);

				panelId++;
			}
		}
	}

	static bool isGlueModule(rack::Module *module) {
		if (!module || !module->model || !module->model->plugin)
			return false;
		return module->model->plugin->slug == "Stoermelder-P1" && module->model->slug == "Glue";
	}

	static void addHubModuleToMapping(auto *module, std::vector<BrandModule> &moduleData) {
		int64_t moduleID = module->getId();
		auto brand_module = ModuleDirectory::convertSlugs(module);
		auto moduleWidget = APP->scene->rack->getModule(moduleID);
		if (moduleWidget) {
			moduleData.push_back(
				{moduleID, brand_module.c_str(), moduleWidget->getBox().getLeft(), moduleWidget->getBox().getTop()});
			if (module->model->slug.size() > 31) {
				pr_warn("Warning: module slug truncated to 31 chars\n");
			}
		}
	}

	static void addModuleToMapping(auto *module,
								   std::vector<BrandModule> &moduleData,
								   std::vector<ParamMap> &paramData,
								   MIDI::Modules &midimodules,
								   ExpanderMappings &expanders) {

		if (ModuleDirectory::isRegularModule(module)) {
			int64_t moduleID = module->getId();
			auto brand_module = ModuleDirectory::convertSlugs(module);
			auto moduleWidget = APP->scene->rack->getModule(moduleID);
			if (moduleWidget) {
				moduleData.push_back({moduleID,
									  brand_module.c_str(),
									  moduleWidget->getBox().getLeft(),
									  moduleWidget->getBox().getTop()});
				if (module->model->slug.size() > 31) {
					pr_warn("Warning: module slug truncated to 31 chars\n");
				}
			}

			if (!ModuleDirectory::isHubOrExpander(module)) {
				for (size_t i = 0; i < module->paramQuantities.size(); i++) {
					float val = module->getParamQuantity(i)->getScaledValue();
					paramData.push_back({.value = val, .paramID = (int)i, .moduleID = moduleID});
				}
			}
		}
		midimodules.addMidiModule(module);
		expanders.addModule(module);
	}

	static void writeToFile(const std::string &fileName, std::string textToWrite) {
		std::ofstream myfile;
		myfile.open(fileName);
		myfile << textToWrite;
		myfile.close();
	}

	static void writeAsHeader(const std::string &fileName, std::string_view structname, std::string_view textToWrite) {
		std::ofstream myfile;
		myfile.open(fileName);
		myfile << "static char " << structname << "[] = \n";
		myfile << "R\"(\n";
		myfile << textToWrite;
		myfile << "\n)\";";
		myfile.close();
	}

	void writeBinaryFile(const std::string &fileName, const std::vector<unsigned char> data) {
		std::ofstream myfile{fileName, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc};
		myfile.write(reinterpret_cast<const char *>(data.data()), data.size());
		myfile.close();
	}
};

} // namespace MetaModule
