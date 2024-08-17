#include "mapping/midi_modules.hh"
#include "mapping/JackMap.hh"
#include "mapping/module_directory.hh"
#include "patch/midi_def.hh"
#include <optional>

namespace MetaModule::MIDI
{
static std::optional<MidiCVSettings> readMidiCVModule(int64_t module_id);
static std::optional<MidiGateSettings> readMidiGateModule(int64_t module_id);
static std::optional<MidiCCCVSettings> readMidiCCCVModule(int64_t module_id);
static std::optional<MidiCCKnobSettings> readMidiMapModule(int64_t module_id);

void Modules::addMidiModule(rack::Module *module) {
	if (module->model->slug == "MIDIToCVInterface") {
		auto read_settings = readMidiCVModule(module->id);
		if (read_settings) {
			settings.CV = read_settings.value();
			moduleIds.midiCV = module->id;
		}
	} else if (module->model->slug == "MIDI-Map") {
		auto read_settings = readMidiMapModule(module->id);
		if (read_settings) {
			settings.CCKnob = read_settings.value();
			moduleIds.midiMaps = module->id;
		}
	} else if (module->model->slug == "MIDITriggerToCVInterface") {
		auto read_settings = readMidiGateModule(module->id);
		if (read_settings) {
			settings.gate = read_settings.value();
			moduleIds.midiGate = module->id;
		}
	} else if (module->model->slug == "MIDICCToCVInterface") {
		auto read_settings = readMidiCCCVModule(module->id);
		if (read_settings) {
			settings.CCCV = read_settings.value();
			moduleIds.midiCC = module->id;
		}
	}
}

// Check if cable goes out from MIDICV module to a Split module
void Modules::addPolySplitCable(rack::Cable *cable) {
	auto out = cable->outputModule;
	auto in = cable->inputModule;

	if (out->getId() == moduleIds.midiCV && in->model->slug == "Split") {
		if (cable->outputId == CoreMidiJacks::VoctJack)
			settings.CV.voctSplitModuleId = in->getId();

		else if (cable->outputId == CoreMidiJacks::GateJack)
			settings.CV.gateSplitModuleId = in->getId();

		else if (cable->outputId == CoreMidiJacks::VelJack)
			settings.CV.velSplitModuleId = in->getId();

		else if (cable->outputId == CoreMidiJacks::AftJack)
			settings.CV.aftSplitModuleId = in->getId();

		else if (cable->outputId == CoreMidiJacks::RetrigJack)
			settings.CV.retrigSplitModuleId = in->getId();
	}
}

bool Modules::isPolySplitModule(rack::Module *module) {
	auto id = module->getId();

	return (id > 0) && ((id == settings.CV.voctSplitModuleId) || (id == settings.CV.gateSplitModuleId) ||
						(id == settings.CV.velSplitModuleId) || (id == settings.CV.aftSplitModuleId) ||
						(id == settings.CV.retrigSplitModuleId));
}

unsigned clockDivToMidiClockJack(unsigned clockDiv) {
	return clockDiv == 0  ? MidiClockJack :
		   clockDiv == 1  ? MidiClockJack :
		   clockDiv == 2  ? MidiClockDiv2Jack :
		   clockDiv == 3  ? MidiClockDiv3Jack :
		   clockDiv == 6  ? MidiClockDiv6Jack :
		   clockDiv == 12 ? MidiClockDiv12Jack :
		   clockDiv == 24 ? MidiClockDiv24Jack :
		   clockDiv == 48 ? MidiClockDiv48Jack :
		   clockDiv == 96 ? MidiClockDiv96Jack :
							MidiClockDiv96Jack;
}

std::optional<MidiCVSettings> readMidiCVModule(int64_t module_id) {
	auto context = rack::contextGet();
	auto engine = context->engine;
	auto module = engine->getModule(module_id);
	if (!module)
		return std::nullopt;

	json_t *rootJ = module->dataToJson();
	if (!rootJ)
		return std::nullopt;

	json_t *pwRangeJ = json_object_get(rootJ, "pwRange");
	json_t *channelsJ = json_object_get(rootJ, "channels");
	json_t *polyModeJ = json_object_get(rootJ, "polyMode");
	json_t *clockDivisionJ = json_object_get(rootJ, "clockDivision");

	MidiCVSettings settings;
	settings.pwRange = pwRangeJ ? json_number_value(pwRangeJ) : -1;
	settings.channels = channelsJ ? json_integer_value(channelsJ) : 1;
	settings.clockDivJack =
		clockDivisionJ ? clockDivToMidiClockJack(json_integer_value(clockDivisionJ)) : unsigned(MidiClockDiv96Jack);

	auto polyMode = polyModeJ ? json_integer_value(polyModeJ) : 0;
	if (polyMode >= 0 && polyMode <= 3)
		settings.polyMode = static_cast<MidiCVSettings::PolyMode>(polyMode);
	else
		settings.polyMode = MidiCVSettings::ROTATE_MODE;

	json_decref(rootJ);
	return settings;
}

std::optional<MidiGateSettings> readMidiGateModule(int64_t module_id) {
	auto context = rack::contextGet();
	auto engine = context->engine;
	auto module = engine->getModule(module_id);
	if (!module)
		return std::nullopt;

	MidiGateSettings settings;

	json_t *rootJ = module->dataToJson();
	if (!rootJ)
		return std::nullopt;

	json_t *notesJ = json_object_get(rootJ, "notes");
	json_t *velocityJ = json_object_get(rootJ, "velocity");
	json_t *mpeModeJ = json_object_get(rootJ, "mpeMode");

	if (!notesJ)
		return std::nullopt;

	for (int i = 0; auto &note : settings.notes) {
		json_t *noteJ = json_array_get(notesJ, i);
		if (noteJ) {
			note = json_integer_value(noteJ);
		}
		i++;
	}
	settings.velocity_mode = velocityJ ? json_boolean_value(velocityJ) : false;
	settings.mpe_mode = mpeModeJ ? json_boolean_value(mpeModeJ) : false;

	json_decref(rootJ);

	return settings;
}

std::optional<MidiCCCVSettings> readMidiCCCVModule(int64_t module_id) {
	auto context = rack::contextGet();
	auto engine = context->engine;
	auto module = engine->getModule(module_id);
	if (!module)
		return std::nullopt;

	json_t *rootJ = module->dataToJson();
	if (!rootJ)
		return std::nullopt;

	json_t *ccsJ = json_object_get(rootJ, "ccs");
	json_t *valuesJ = json_object_get(rootJ, "values");
	json_t *smoothJ = json_object_get(rootJ, "smooth");
	json_t *mpeModeJ = json_object_get(rootJ, "mpeMode");
	json_t *lsbModeJ = json_object_get(rootJ, "lsbMode");

	MidiCCCVSettings settings;

	for (int i = 0; auto &cc : settings.CCnums) {
		json_t *ccJ = json_array_get(ccsJ, i);
		cc = ccJ ? json_integer_value(ccJ) : -1;
		i++;
	}

	for (int i = 0; auto &val : settings.values) {
		json_t *valJ = json_array_get(valuesJ, i);
		val = valJ ? json_integer_value(valJ) : 0;
		i++;
	}

	settings.smooth = smoothJ ? json_boolean_value(smoothJ) : false;
	settings.mpe_mode = mpeModeJ ? json_boolean_value(mpeModeJ) : false;
	settings.lsb_mode = lsbModeJ ? json_boolean_value(lsbModeJ) : false;

	json_decref(rootJ);

	return settings;
}

std::optional<MidiCCKnobSettings> readMidiMapModule(int64_t module_id) {
	auto context = rack::contextGet();
	auto engine = context->engine;
	auto module = engine->getModule(module_id);
	if (!module)
		return std::nullopt;

	json_t *rootJ = module->dataToJson();
	if (!rootJ)
		return std::nullopt;

	MidiCCKnobSettings settings;

	json_t *smoothJ = json_object_get(rootJ, "smooth");
	json_t *mapsJ = json_object_get(rootJ, "maps");

	if (mapsJ) {
		auto num_maps = std::min<size_t>(json_array_size(mapsJ), settings.ccs.size());

		for (size_t i = 0; i < num_maps; i++) {
			json_t *mapJ = json_array_get(mapsJ, i);
			if (!mapJ)
				break;

			json_t *ccJ = json_object_get(mapJ, "cc");
			json_t *moduleIdJ = json_object_get(mapJ, "moduleId");
			json_t *paramIdJ = json_object_get(mapJ, "paramId");
			if (!(ccJ && moduleIdJ && paramIdJ))
				continue;

			settings.ccs[i].CCnum = json_integer_value(ccJ);
			settings.ccs[i].module_id = json_integer_value(moduleIdJ);
			settings.ccs[i].param_id = json_integer_value(paramIdJ);
		}
	}

	settings.smooth = smoothJ ? json_boolean_value(smoothJ) : false;

	json_decref(rootJ);

	return settings;
}

} // namespace MetaModule::MIDI
