#pragma once
#include "console/pr_dbg.hh"
#include "cpputil/util/colors.hh"
#include "hub/hub_knob_mappings.hh"
#include "mapping/JackMap.hh"
#include "mapping/ModuleID.h"
#include "mapping/ParamMap.hh"
#include "mapping/midi_modules.hh"
#include "mapping/module_directory.hh"
#include "mapping/patch_writer.hh"
#include "patch/midi_def.hh"
#include <fstream>
#include <rack.hpp>

// Adpats VCVRack-format of patch data to a format PatchFileWriter can use
template<size_t NumKnobs, size_t MaxMapsPerPot, size_t MaxKnobSets>
struct VCVPatchFileWriter {
	using HubKnobsMultiMap = typename HubKnobMappings<NumKnobs, MaxMapsPerPot, MaxKnobSets>::HubKnobsMultiMaps;

	static void writePatchFile(int64_t hubModuleId,
							   HubKnobsMultiMap &mappings,
							   std::span<std::string> knobSetNames,
							   std::string fileName,
							   std::string patchName,
							   std::string patchDesc) {

		auto context = rack::contextGet();
		auto engine = context->engine;

		// Find all compatible modules in this patch
		// TODO: only add modules that are mapped to this hub
		// Find all knobs on those modules (static knobs)
		std::vector<BrandModule> moduleData;
		std::vector<ParamMap> paramData;
		std::vector<int64_t> splitModuleIds;
		MetaModule::MIDI::Modules midimodules;

		auto moduleIDs = engine->getModuleIds();
		for (auto moduleID : moduleIDs) {

			auto *module = engine->getModule(moduleID);

			if (ModuleDirectory::isRegularModule(module)) {
				auto brand_module = ModuleDirectory::convertSlugs(module);
				moduleData.push_back({moduleID, brand_module.c_str()});
				if (module->model->slug.size() > 31)
					pr_dbg("Warning: module slug truncated to 31 chars\n");

				if (!ModuleDirectory::isHub(module)) {
					for (size_t i = 0; i < module->paramQuantities.size(); i++) {
						float val = module->getParamQuantity(i)->getScaledValue();
						paramData.push_back({.value = val, .paramID = (int)i, .moduleID = moduleID});
					}
				}
			}
			midimodules.addMidiModule(module);
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

			bool isMidiOutput = ModuleDirectory::isCoreMIDI(out) || midimodules.isPolySplitModule(out);
			bool isKnownOutModule = ModuleDirectory::isRegularModule(out) || isMidiOutput;

			// The output module must be in the plugin, or a Core MIDI module, or a Split module connected to a Core MIDI module.
			// The input module must be in the plugin.
			if (!(isKnownOutModule && ModuleDirectory::isRegularModule(in)))
				continue;

			// Ignore cables that are connected to a different hub
			if (ModuleDirectory::isHub(out) && (out->getId() != hubModuleId))
				continue;
			if (ModuleDirectory::isHub(in) && (in->getId() != hubModuleId))
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

		PatchFileWriter pw{moduleData, hubModuleId};
		pw.setMidiSettings(midimodules.moduleIds, midimodules.settings);
		pw.setPatchName(patchName);
		pw.setPatchDesc(patchDesc);
		pw.setCableList(cableData);
		pw.setParamList(paramData);

		// Add module state from Module::dataToJson()
		for (auto moduleID : engine->getModuleIds()) {
			auto *module = engine->getModule(moduleID);
			if (ModuleDirectory::isRegularModule(module) && !ModuleDirectory::isHub(module)) {
				pw.addModuleStateJson(module);
			}
		}

		// Iterate mappings, by MaxKnobSets times
		for (unsigned set_i = 0; set_i < MaxKnobSets; set_i++) {
			pw.addKnobMapSet(set_i, knobSetNames[set_i]);

			for (unsigned hubParamId = 0; auto &knob_maps : mappings) {

				std::vector<Mapping> active_maps;
				active_maps.reserve(8);

				for (auto &mapsets : knob_maps) {
					auto &map = mapsets.maps[set_i];
					if (map.moduleId > 0)
						active_maps.push_back(map);
				}

				if (active_maps.size())
					pw.addKnobMaps(hubParamId, set_i, active_maps);

				hubParamId++;
			}
		}

		std::string yml = pw.printPatchYAML();
		writeToFile(fileName, yml);
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
