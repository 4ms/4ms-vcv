#pragma once
#include "mapping/ModuleID.h"
#include <rack.hpp>

struct ModuleDirectory {

	// Return false if the module should not be included in the patch
	// Including a module that can't be loaded will give an warning
	// message the user when they try to load the patch on hardware.
	// This can be good feedback. Only exclude modules which users
	// would not expect to have running on hardware (because returning false
	// here will surpress the warning message).
	static bool isInPlugin(rack::Module *module) {
		if (!module)
			return false;
		if (!module->model)
			return false;
		if (!module->model->plugin)
			return false;

		std::array blacklist = {"Scope",
								"Oscilloscope",
								"AudioInterface2",
								"AudioInterface",
								"AudioInterface16",
								"Bogaudio-Analyzer",
								"Bogaudio-AnalyzerXL",
								"Bogaudio-Ranalyzer",
								"Notes",
								"Blank",
								"MIDIToCVInterface",
								"MIDI-Map",
								"MIDITriggerToCVInterface",
								"MIDICCToCVInterface",
								"Split"

		};

		for (auto slug : blacklist) {
			if (module->model->slug == slug)
				return false;
		}
		return true;
	}

	static bool isHub(std::string_view slug) {
		if (slug == "PanelMedium")
			return true;
		if (slug == "HubMedium")
			return true;
		if (slug == "4msCompany:PanelMedium")
			return true;
		if (slug == "4msCompany:HubMedium")
			return true;

		return false;
	}

	static bool isHub(rack::Module *module) {
		if (!module)
			return false;
		if (!module->model)
			return false;
		if (!module->model->plugin)
			return false;

		if (module->model->plugin->slug != "4msCompany")
			return false;

		return isHub(module->model->slug);
	}

	static bool isModuleInPlugin(rack::Module *module) {
		return isInPlugin(module) && !isHub(module);
	}

	static bool isCoreMIDI(rack::Module *module) {
		if (!module)
			return false;
		if (!module->model)
			return false;
		if (!module->model->plugin)
			return false;

		if (module->model->plugin->slug == "Core") {
			if (module->model->slug == "MIDIToCVInterface")
				return true;
			if (module->model->slug == "MIDICCToCVInterface")
				return true;
			if (module->model->slug == "MIDI-Map")
				return true;
			if (module->model->slug == "MIDITriggerToCVInterface")
				return true;
		}

		return false;
	}

	static bool isInPluginOrMIDI(rack::Module *module) {
		return isInPlugin(module) || isCoreMIDI(module);
	}
};
