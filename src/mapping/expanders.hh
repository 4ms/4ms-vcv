#pragma once
#include "CoreModules/hub/audio_expander_defs.hh"
#include "CoreModules/hub/panel_medium_defs.hh"
#include "mapping/JackMap.hh"
#include "mapping/module_directory.hh"
#include <cstdint>
#include <rack.hpp>

namespace MetaModule
{

struct ExpanderMappings {
private:
	struct Audio {
		int64_t module_id = -1;
	};

	Audio audio_exp{};

public:
	void addModule(rack::Module *module) {
		if (!module)
			return;

		if (ModuleDirectory::isAudioExpander(module)) {
			if (hasAudioExpander())
				WARN("Multiple Audio Expanders found: only one will be used\n");

			audio_exp.module_id = module->id;
		}
	}

	int64_t getAudioExpanderId() const {
		return audio_exp.module_id;
	}

	bool hasAudioExpander() {
		return audio_exp.module_id > -1;
	}

	// returns true if cable is connected to an Expander module
	// that we don't know about (e.g. patch contains more than the max
	// number of a cetain type of expander)
	bool isUnknownExpanderCable(rack::Cable *cable) {
		if (!cable)
			return true;

		if (ModuleDirectory::isAudioExpander(cable->inputModule)) {
			return (cable->inputModule->getId() != audio_exp.module_id);
		}
		if (ModuleDirectory::isAudioExpander(cable->outputModule)) {
			return (cable->outputModule->getId() != audio_exp.module_id);
		}

		return false;
	}

	bool isKnownJackExpander(int64_t module_id) {
		return module_id == audio_exp.module_id;
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
