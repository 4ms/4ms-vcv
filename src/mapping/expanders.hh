#pragma once
#include "CoreModules/hub/audio_expander_defs.hh"
#include "CoreModules/hub/panel_medium_defs.hh"
#include "mapping/JackMap.hh"
#include "mapping/module_directory.hh"
#include <cstdint>
#include <rack.hpp>
#include <vector>

namespace MetaModule
{

struct ExpanderMappings {
private:
	int64_t audio_exp_module_id = -1;

	constexpr static size_t MaxButtonExpanders = 4;
	struct ButtonExp {
		int64_t module_id = -1;
		// MetaModuleHubBase *module = nullptr;
	};
	std::vector<ButtonExp> button_exps{};

public:
	void addModule(rack::Module *module) {
		if (!module)
			return;

		if (ModuleDirectory::isAudioExpander(module)) {
			if (audio_exp_module_id > -1)
				WARN("Multiple Audio Expanders found: only one will be used\n");

			audio_exp_module_id = module->id;
		}

		if (ModuleDirectory::isButtonExpander(module)) {
			if (button_exps.size() >= MaxButtonExpanders)
				WARN("Found more than %zu Button Expanders\n", MaxButtonExpanders);

			button_exps.push_back(ButtonExp{.module_id = module->id});
		}
    }
    
	int64_t getAudioExpanderId() const {
		return audio_exp_module_id;
	}

	


	// returns true if cable is connected to an Expander module
	// that we don't know about (e.g. patch contains more than the max
	// number of a certain type of expander)
	bool isUnknownExpanderCable(rack::Cable *cable) {
		if (!cable)
			return true;

		if (ModuleDirectory::isAudioExpander(cable->inputModule)) {
			return (cable->inputModule->getId() != audio_exp_module_id);
		}
		if (ModuleDirectory::isAudioExpander(cable->outputModule)) {
			return (cable->outputModule->getId() != audio_exp_module_id);
		}

		return false;
	}

	bool isKnownJackExpander(int64_t module_id) {
		return module_id == audio_exp_module_id;
	}

	// Adjusts the cable jack ID from the expander's internal ID
	// to the panel mapping ID
	void setExpanderInputJackId(CableMap *cable) {
		cable->sendingJackId = AudioExpander::get_map_injack_num(cable->sendingJackId);
	}

	void setExpanderOutputJackId(CableMap *cable) {
		cable->receivedJackId = AudioExpander::get_map_outjack_num(cable->receivedJackId);
	}
};

} // namespace MetaModule
