#include "patch_writer.hh"
#include "console/pr_dbg.hh"
#include "hub/jack_alias.hh"
#include "mapping/midi_modules.hh"
#include "mapping/module_directory.hh"
#include "patch-serial/patch_to_yaml.hh"
#include <algorithm>

namespace MetaModule
{

PatchFileWriter::PatchFileWriter(std::vector<BrandModule> modules, int64_t hubModuleId)
	: hubModuleId{hubModuleId} {
	setModuleList(modules);
	pd.knob_sets.clear();
	pd.mapped_ins.clear();
	pd.mapped_outs.clear();
}

PatchFileWriter::~PatchFileWriter() = default;

void PatchFileWriter::setPatchName(std::string patchName) {
	pd.patch_name = patchName.c_str();
}

void PatchFileWriter::setPatchDesc(std::string patchDesc) {
	pd.description = patchDesc.c_str();
}

void PatchFileWriter::setMidiSettings(MIDI::Settings const &settings) {
	midiSettings = settings;

	// Handle MIDI CC->Knob maps

	for (auto const &ccknob_module : midiSettings.CCKnob) {
		for (auto cc : ccknob_module.ccs) {
			if (cc.CCnum > 0 && idMap.contains(cc.module_id)) {
				pd.midi_maps.set.emplace_back(MappedKnob{
					.panel_knob_id = (uint16_t)(cc.CCnum + MidiCC0),
					.module_id = idMap[cc.module_id],
					.param_id = cc.param_id,
					.curve_type = 0,
					.midi_chan = ccknob_module.midi_chan,
					.min = 0,
					.max = 1.f,
					.alias_name = "",
				});
			}
		}
	}
	if (pd.midi_maps.set.size()) {
		pd.midi_maps.name = "MIDI";
	}

	if (midiSettings.CV.size()) {
		// TODO: make these per MIDI channel
		pd.midi_poly_num = std::ranges::max_element(midiSettings.CV, {}, &MIDI::MidiCVSettings::channels)->channels;
		pd.midi_poly_num = std::min(pd.midi_poly_num, 8u);
		pd.midi_poly_mode = midiSettings.CV[0].polyMode;
		pd.midi_pitchwheel_range = midiSettings.CV[0].pitchwheelRange;
	} else {
		pd.midi_poly_num = 1;
		pd.midi_poly_mode = PolyMode::Rotate;
		pd.midi_pitchwheel_range = -1;
	}
}

void PatchFileWriter::setExpanders(ExpanderMappings const &exp) {
	expanders = exp;
}

void PatchFileWriter::setModuleList(std::vector<BrandModule> &modules) {
	std::vector<int64_t> vcv_mod_ids;

	std::sort(modules.begin(), modules.end(), [&](auto const &a, auto const &b) {
		return a.y < b.y || (a.y == b.y && a.x < b.x);
	});

	// Reserved for PANEL
	vcv_mod_ids.push_back(-1);
	pd.module_slugs.push_back("");

	for (auto &mod : modules) {
		if (ModuleDirectory::isHub(mod.slug)) {
			pd.module_slugs[0] = mod.slug;
			vcv_mod_ids[0] = mod.id;
		} else {
			pd.module_slugs.push_back(mod.slug);
			vcv_mod_ids.push_back(mod.id);
		}
	}
	if (vcv_mod_ids[0] < 0)
		return;
	// error: no panel!

	idMap = squash_ids(vcv_mod_ids);
}

void PatchFileWriter::setCableList(std::vector<CableMap> &cables) {
	pd.int_cables.clear();

	for (auto &cable : cables) {
		auto in_jack = cable.inputJackId;
		auto out_jack = cable.outputJackId;

		if (out_jack < 0 || in_jack < 0 || cable.outputModuleId < 0 || cable.inputModuleId < 0)
			continue;

		bool handled = false;

		for (auto const &cv_module : midiSettings.CV) {

			if (cable.outputModuleId == cv_module.voctSplitModuleId) {
				mapMidiCVPolySplitJack(cable, MidiMonoNoteJack, cv_module.midi_chan);
				handled = true;

			} else if (cable.outputModuleId == cv_module.gateSplitModuleId) {
				mapMidiCVPolySplitJack(cable, MidiMonoGateJack, cv_module.midi_chan);
				handled = true;

			} else if (cable.outputModuleId == cv_module.velSplitModuleId) {
				mapMidiCVPolySplitJack(cable, MidiMonoVelJack, cv_module.midi_chan);
				handled = true;

			} else if (cable.outputModuleId == cv_module.aftSplitModuleId) {
				mapMidiCVPolySplitJack(cable, MidiMonoAftertouchJack, cv_module.midi_chan);
				handled = true;

			} else if (cable.outputModuleId == cv_module.retrigSplitModuleId) {
				mapMidiCVPolySplitJack(cable, MidiMonoRetrigJack, cv_module.midi_chan);
				handled = true;

			} else if (cable.outputModuleId == cv_module.module_id) {
				mapMidiCVJack(cable, cv_module.midi_chan);
				handled = true;
			}
		}

		if (handled)
			continue; // next cable

		for (auto const &gate_module : midiSettings.gate) {
			if (cable.outputModuleId == gate_module.module_id) {
				mapMidiGateJack(cable, gate_module.midi_chan);
				handled = true;
			}
		}
		if (handled)
			continue; // next cable

		for (auto const &cccv_module : midiSettings.CCCV) {
			if (cable.outputModuleId == cccv_module.module_id) {
				mapMidiCCJack(cable, cccv_module.midi_chan);
				handled = true;
			}
		}
		if (handled)
			continue; // next cable

		for (auto const &ccknob_module : midiSettings.CCKnob) {
			if (cable.outputModuleId == ccknob_module.module_id) {
				//MIDI Maps has no jacks
				handled = true;
			}
		}
		if (handled)
			continue; // next cable

		if (cable.outputModuleId == hubModuleId) {
			mapInputJack(cable);
			continue;

		} else if (cable.inputModuleId == hubModuleId) {
			mapOutputJack(cable);
			continue;

		} else if (expanders.isKnownJackExpander(cable.outputModuleId)) {
			mapInputJack(cable);
			continue;

		} else if (expanders.isKnownJackExpander(cable.inputModuleId)) {
			mapOutputJack(cable);
			continue;
		}

		if (!idMap.contains(cable.inputModuleId) || !idMap.contains(cable.outputModuleId))
			continue;

		// Map internal cable:

		auto in_mod = idMap[cable.inputModuleId];
		auto out_mod = idMap[cable.outputModuleId];

		// Look for an existing entry:
		auto found = std::find_if(pd.int_cables.begin(), pd.int_cables.end(), [=](const auto &x) {
			return x.out.jack_id == out_jack && x.out.module_id == out_mod;
		});

		if (found != pd.int_cables.end()) {
			// If an int_cable entry exists for this output jack, add a new input jack to the ins vector
			found->ins.push_back({
				.module_id = static_cast<uint16_t>(in_mod),
				.jack_id = static_cast<uint16_t>(in_jack),
			});
		} else {
			// Make a new entry:
			pd.int_cables.push_back({
				.out = {static_cast<uint16_t>(out_mod), static_cast<uint16_t>(out_jack)},
				.ins = {{
					{
						.module_id = static_cast<uint16_t>(in_mod),
						.jack_id = static_cast<uint16_t>(in_jack),
					},
				}},
				.color = cable.lv_color_full,
			});
		}
	}
}

void PatchFileWriter::setJackAliases(JackAlias &aliases) {
	for (auto &i : pd.mapped_ins) {
		if (i.panel_jack_id < aliases.in.size()) {
			i.alias_name = aliases.in[i.panel_jack_id].c_str();
		}
	}

	for (auto &i : pd.mapped_outs) {
		if (i.panel_jack_id < aliases.out.size()) {
			i.alias_name = aliases.out[i.panel_jack_id].c_str();
		}
	}
}

void PatchFileWriter::setParamList(std::vector<ParamMap> &params) {
	pd.static_knobs.clear();
	for (auto &param : params) {
		if (idMap.contains(param.moduleID)) {
			pd.static_knobs.push_back({
				.module_id = idMap[param.moduleID],
				.param_id = static_cast<uint16_t>(param.paramID),
				.value = param.value,
			});
		}
	}
}

void PatchFileWriter::addModuleStateJson(rack::Module *module) {
	if (!module)
		return; //invalid module

	if (!idMap.contains(module->id))
		return; //module not recognized

	if (json_t *dataJ = module->dataToJson(); dataJ) {
		std::string state_string;
		auto sz = json_dumpb(dataJ, nullptr, 0, JSON_COMPACT);
		if (sz > 0) {
			state_string.resize(sz, '\0');
			json_dumpb(dataJ, (char *)state_string.data(), sz, JSON_COMPACT);
		} else {
			if (auto state_cstr = json_string_value(dataJ); state_cstr) {
				state_string = state_cstr;
			}
		}
		json_decref(dataJ);

		if (state_string.size() > 0)
			pd.module_states.push_back({idMap[module->id], state_string});
	}
}

void PatchFileWriter::addKnobMapSet(unsigned knobSetId, std::string_view knobSetName) {
	if (knobSetId >= pd.knob_sets.size())
		pd.knob_sets.resize(knobSetId + 1);
	pd.knob_sets[knobSetId].name = knobSetName;
}

void PatchFileWriter::addKnobMaps(unsigned panelKnobId, unsigned knobSetId, const std::span<const Mapping> maps) {
	if (knobSetId >= pd.knob_sets.size())
		pd.knob_sets.resize(knobSetId + 1);

	for (const auto &m : maps) {
		if (!idMap.contains(m.moduleId)) {
			pr_dbg("Skipping knob mapping to module not supported by MetaModule: %lld\n", (long long)m.moduleId);
			continue;
		}
		pd.knob_sets[knobSetId].set.push_back({
			.panel_knob_id = static_cast<uint16_t>(panelKnobId),
			.module_id = idMap[m.moduleId],
			.param_id = static_cast<uint16_t>(m.paramId),
			.curve_type = 0,
			.midi_chan = 0,
			.min = m.range_min,
			.max = m.range_max,
			.alias_name = m.alias_name.c_str(),
		});
	}
}

// Presumes the map has already been verified that the outputModuleId is
// the hub we're using or a known expander,
// and the jack ids are valid, and the receivedModuleId is in our module list
void PatchFileWriter::mapInputJack(CableMap &map) {
	if (expanders.isKnownJackExpander(map.outputModuleId))
		expanders.setExpanderInputJackId(&map);

	if (expanders.isKnownJackExpander(map.inputModuleId))
		expanders.setExpanderOutputJackId(&map);

	if (idMap.contains(map.inputModuleId)) {
		// Look for an existing entry to this panel input jack
		auto found = std::find_if(pd.mapped_ins.begin(), pd.mapped_ins.end(), [=](const auto &x) {
			return x.panel_jack_id == (uint32_t)map.outputJackId;
		});

		if (found != pd.mapped_ins.end()) {
			// If we already have an entry for this panel jack, append a new module input jack to the ins vector
			found->ins.push_back({
				.module_id = static_cast<uint16_t>(idMap[map.inputModuleId]),
				.jack_id = static_cast<uint16_t>(map.inputJackId),
			});
		} else {
			// Make a new entry:
			pd.mapped_ins.push_back({
				.panel_jack_id = static_cast<uint32_t>(map.outputJackId),
				.ins = {{
					{
						.module_id = static_cast<uint16_t>(idMap[map.inputModuleId]),
						.jack_id = static_cast<uint16_t>(map.inputJackId),
					},
				}},
				.alias_name = "",
			});
		}
	}
}

// Presumes the map has already been verified that the outputModuleId is the hub we're using
// And the jack ids are valid
// and the receivedModuleId is in our module list
void PatchFileWriter::mapOutputJack(CableMap &map) {
	if (expanders.isKnownJackExpander(map.outputModuleId))
		expanders.setExpanderInputJackId(&map);

	if (expanders.isKnownJackExpander(map.inputModuleId))
		expanders.setExpanderOutputJackId(&map);

	// Update the mapped_outs entry if there already is one with the same panel_jack_id (Note that this is
	// an error, since we can't have multiple outs assigned to a net, but we're going to roll with it).
	// otherwise push it to the vector

	if (idMap.contains(map.outputModuleId)) {
		// Look for an existing entry:
		auto found = std::find_if(pd.mapped_outs.begin(), pd.mapped_outs.end(), [=](const auto &x) {
			return x.panel_jack_id == (uint32_t)map.inputJackId;
		});

		if (found != pd.mapped_outs.end()) {
			found->out.module_id = static_cast<uint16_t>(idMap[map.outputModuleId]);
			found->out.jack_id = static_cast<uint16_t>(map.outputJackId);
			// Todo: Log error: multiple module outputs mapped to same panel output jack
		} else {
			// Make a new entry:
			pd.mapped_outs.push_back({
				.panel_jack_id = static_cast<uint32_t>(map.inputJackId),
				.out =
					{
						.module_id = static_cast<uint16_t>(idMap[map.outputModuleId]),
						.jack_id = static_cast<uint16_t>(map.outputJackId),
					},
				.alias_name = "",
			});
		}
	}
}

void PatchFileWriter::mapMidiCVPolySplitJack(CableMap &cable, unsigned monoJackId, unsigned midi_chan) {
	if (cable.outputJackId >= 8) {
		return; //skip poly > 8
	}

	cable.outputJackId = MetaModule::Midi::set_midi_channel(monoJackId + cable.outputJackId, midi_chan);
	mapInputJack(cable);
}

void PatchFileWriter::mapMidiCVJack(CableMap &cable, uint32_t midi_chan) {
	using enum MIDI::CoreMidiJacks;

	if (cable.outputJackId == VoctJack)
		cable.outputJackId = MidiMonoNoteJack;

	else if (cable.outputJackId == GateJack)
		cable.outputJackId = MidiMonoGateJack;

	else if (cable.outputJackId == VelJack)
		cable.outputJackId = MidiMonoVelJack;

	else if (cable.outputJackId == AftJack)
		cable.outputJackId = MidiMonoAftertouchJack;

	else if (cable.outputJackId == RetrigJack)
		cable.outputJackId = MidiMonoRetrigJack;

	else if (cable.outputJackId == PWJack)
		cable.outputJackId = MidiPitchWheelJack;

	else if (cable.outputJackId == MWJack)
		cable.outputJackId = MidiModWheelJack;

	else if (cable.outputJackId == ClockJack)
		cable.outputJackId = MidiClockJack;

	else if (cable.outputJackId == ClockDivJack) {

		for (auto const &cv_module : midiSettings.CV) {
			if (cv_module.module_id == cable.outputModuleId) {
				cable.outputJackId = std::clamp<unsigned>(cv_module.clockDivJack, MidiClockJack, MidiClockDiv96Jack);
			}
		}
	}

	else if (cable.outputJackId == StartJack)
		cable.outputJackId = MidiStartJack;

	else if (cable.outputJackId == StopJack)
		cable.outputJackId = MidiStopJack;

	else if (cable.outputJackId == ContJack)
		cable.outputJackId = MidiContinueJack;

	cable.outputJackId = MetaModule::Midi::set_midi_channel(cable.outputJackId, midi_chan);
	mapInputJack(cable);
}

void PatchFileWriter::mapMidiGateJack(CableMap &cable, unsigned midi_chan) {
	for (auto const &gate_module : midiSettings.gate) {
		if (cable.outputJackId <= (int)gate_module.notes.size()) {
			auto notenum = gate_module.notes[cable.outputJackId];
			cable.outputJackId = MetaModule::Midi::set_midi_channel(MidiGateNote0 + notenum, midi_chan);
			mapInputJack(cable);
		}
	}
}

void PatchFileWriter::mapMidiCCJack(CableMap &cable, unsigned midi_chan) {
	for (auto const &cccv_module : midiSettings.CCCV) {
		if (cable.outputJackId <= (int)cccv_module.CCnums.size()) {
			auto ccnum = cccv_module.CCnums[cable.outputJackId];

			cable.outputJackId = MetaModule::Midi::set_midi_channel(MidiCC0 + ccnum, midi_chan);
			mapInputJack(cable);
		}
	}
}

std::string PatchFileWriter::printPatchYAML() {
	return patch_to_yaml_string(pd);
}

std::map<int64_t, uint16_t> PatchFileWriter::squash_ids(std::vector<int64_t> ids) {
	std::map<int64_t, uint16_t> s;

	int i = 0;
	for (auto id : ids) {
		s[id] = i++;
	}
	return s;
}

PatchData &PatchFileWriter::get_data() {
	return pd;
}

} // namespace MetaModule
