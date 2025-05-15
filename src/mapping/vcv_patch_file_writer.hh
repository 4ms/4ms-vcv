#pragma once
#include "console/pr_dbg.hh"
#include "cpputil/util/colors.hh"
#include "hub/hub_knob_mappings.hh"
#include "mapping/JackMap.hh"
#include "mapping/ModuleID.h"
#include "mapping/ParamMap.hh"
#include "mapping/expanders.hh"
#include "mapping/midi_modules.hh"
#include "mapping/module_directory.hh"
#include "mapping/patch_writer.hh"
#include "patch/midi_def.hh"
#include "plugin.hh"
#include <fstream>
#include <rack.hpp>

namespace MetaModule
{

// Adapts VCVRack-format of patch data to a format PatchFileWriter can use
template<size_t NumKnobs, size_t MaxMapsPerPot, size_t MaxKnobSets>
struct VCVPatchFileWriter {

	static std::string createPatchYml(int64_t hubModuleId,
									  HubKnobMappings<MaxMapsPerPot, MaxKnobSets> &mappings,
									  std::string patchName,
									  std::string patchDesc,
									  MappingMode mappingMode) {

		auto context = rack::contextGet();
		auto engine = context->engine;

		// Find all compatible modules in this patch
		// TODO: only add modules that are mapped to this hub
		// Find all knobs on those modules (static knobs)
		std::vector<BrandModule> moduleData;
		std::vector<ParamMap> paramData;
		std::vector<int64_t> splitModuleIds;
		MIDI::Modules midimodules;
		ExpanderMappings expanders;

		// First, find the hub module and it to the lists
		auto *hubModule = engine->getModule(hubModuleId);
		if (ModuleDirectory::isHub(hubModule)) {
			auto brand_module = ModuleDirectory::convertSlugs(hubModule);
			auto moduleWidget = APP->scene->rack->getModule(hubModuleId);
			if (moduleWidget) {
				moduleData.push_back({hubModuleId,
									  brand_module.c_str(),
									  moduleWidget->getBox().getLeft(),
									  moduleWidget->getBox().getTop()});
				if (hubModule->model->slug.size() > 31) {
					pr_warn("Warning: module slug truncated to 31 chars\n");
				}
			}
			midimodules.addMidiModule(hubModule);
			expanders.addModule(hubModule);
		}

		if (mappingMode == MappingMode::ALL) {
			auto moduleIDs = engine->getModuleIds();
			for (auto moduleID : moduleIDs) {

				auto *module = engine->getModule(moduleID);

				if (ModuleDirectory::isRegularModule(module)) {
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
					midimodules.addMidiModule(module);
					expanders.addModule(module);
				}

			}
		}
		
		if (mappingMode == MappingMode::LEFTRIGHT || mappingMode == MappingMode::LEFT) {

			// Add modules joined to the left of the hub module
			auto* module = engine->getModule(hubModuleId);
			while (true) {
				if (!module) break;

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
					midimodules.addMidiModule(module);
					expanders.addModule(module);
				}

				if (module->leftExpander.moduleId < 0) break;
				if (!module->leftExpander.module) break;
				module = module->leftExpander.module;
			}
		}

		if (mappingMode == MappingMode::LEFTRIGHT || mappingMode == MappingMode::RIGHT) {

			// Add modules joined to the right of the hub module
			auto* module = engine->getModule(hubModuleId);
			while (true) {
				if (!module) break;

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
					midimodules.addMidiModule(module);
					expanders.addModule(module);
				}

				if (module->rightExpander.moduleId < 0) break;
				if (!module->rightExpander.module) break;
				module = module->rightExpander.module;
			}
		}

		// Scan cables for MIDICV -> Split connections
		for (auto cableID : engine->getCableIds()) {
			midimodules.addPolySplitCable(engine->getCable(cableID));
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

				uint8_t r_amt = (uint8_t)(rack::clamp(cableWidget->color.r) * 255);
				uint8_t g_amt = (uint8_t)(rack::clamp(cableWidget->color.g) * 255);
				uint8_t b_amt = (uint8_t)(rack::clamp(cableWidget->color.b) * 255);
				uint16_t color = Color(r_amt, g_amt, b_amt).Rgb565();

				cableData.push_back({
					.sendingJackId = cable->outputId,
					.receivedJackId = cable->inputId,
					.sendingModuleId = out->getId(),
					.receivedModuleId = in->getId(),
					.lv_color_full = color,
				});
			}
		}

		PatchFileWriter pw{moduleData, hubModuleId};
		pw.setMidiSettings(midimodules.moduleIds, midimodules.settings);
		pw.setPatchName(patchName);
		pw.setPatchDesc(patchDesc);
		pw.setExpanders(expanders);
		pw.setCableList(cableData);
		pw.setParamList(paramData);

		// Add module state from Module::dataToJson()
		for (auto moduleID : engine->getModuleIds()) {
			auto *module = engine->getModule(moduleID);
			if (ModuleDirectory::isRegularModule(module)) {
				pw.addModuleStateJson(module);
			}
		}

		// Iterate mappings, by MaxKnobSets times
		for (unsigned set_i = 0; set_i < MaxKnobSets; set_i++) {
			pw.addKnobMapSet(set_i, mappings.knobSetNames[set_i]);

			for (unsigned hubParamId = 0; auto &knob_maps : mappings.mappings) {

				std::vector<Mapping> active_maps;
				active_maps.reserve(8);

				for (auto &mapsets : knob_maps) {
					auto &map = mapsets.maps[set_i];
					map.alias_name = mappings.getMapAliasName({.objID = hubParamId}, set_i);
					if (map.moduleId > 0)
						active_maps.push_back(map);
				}

				if (active_maps.size())
					pw.addKnobMaps(hubParamId, set_i, active_maps);

				hubParamId++;
			}
		}

		return pw.printPatchYAML();
		// writeToFile(fileName, yml);
		// writeAsHeader(fileName + ".hh", patchName + "_patch", yml);
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
