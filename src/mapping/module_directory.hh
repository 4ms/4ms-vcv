#pragma once
#include "mapping/ModuleID.h"
#include <rack.hpp>

struct ModuleDirectory {

	// "Regular" module means it's a virtual module that
	// we should include in the patch file.
	// We exclude modules which users would not expect to have
	// running on hardware, like modules that do MIDI Mappings,
	// notes, blanks, scope modules, etc.
	static bool isRegularModule(rack::Module *module) {
		if (!isValid(module))
			return false;

		if (isHubOrExpander(module))
			return false;

		std::array blacklist = {"AudioInterface2",
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
								"Split",
								"Merge"

		};

		for (auto slug : blacklist) {
			if (module->model->slug == slug)
				return false;
		}
		return true;
	}

	static std::string convertSlugs(rack::Module *module) {
		if (!isValid(module))
			return "";

		auto brand = module->getModel()->plugin->slug;
		auto module_slug = module->getModel()->slug;

		// Convert Airwin2Rack FX selection to a MetaModule module
		if (brand == "Airwin2Rack") {
			brand = "Airwindows";
			auto json = module->dataToJson();
			auto val = json_object_get(json, "airwindowSelectedFX");

			if (val && json_typeof(val) == JSON_STRING)
				module_slug = json_string_value(val);
			else
				module_slug = "Galactic";

			json_decref(json);
		}

		return brand + ":" + module_slug;
	}

	// Hub
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
		if (!isValid(module))
			return false;

		if (module->model->plugin->slug != "4msCompany")
			return false;

		return isHub(module->model->slug);
	}

	// Expanders
	static bool isAudioExpander(std::string_view slug) {
		return slug == "MMAudioExpander";
	}

	static bool isAudioExpander(rack::Module *module) {
		if (!module || !module->model || !module->model->plugin)
			return false;

		if (module->model->plugin->slug != "4msCompany")
			return false;

		return isAudioExpander(module->model->slug);
	}

	static bool isButtonExpander(std::string_view slug) {
		return slug == "MMButtonExpander";
	}

	static bool isButtonExpander(rack::Module *module) {
		if (!module || !module->model || !module->model->plugin)
			return false;

		if (module->model->plugin->slug != "4msCompany")
			return false;

		return isButtonExpander(module->model->slug);
	}

	static bool isExpander(std::string_view slug) {
		return isAudioExpander(slug) || isButtonExpander(slug);
	}

	static bool isExpander(rack::Module *module) {
		return isAudioExpander(module) || isButtonExpander(module);
	}

	static bool isHubOrExpander(rack::Module *module) {
		if (!isValid(module))
			return false;

		if (module->model->plugin->slug != "4msCompany")
			return false;

		return isHub(module->model->slug) || isExpander(module->model->slug);
	}

	// MIDI

	static bool isCoreMIDI(rack::Module *module) {
		if (!isValid(module))
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

	static bool isValid(rack::Module *module) {
		return module && module->model && module->model->plugin;
	}
};
