#include "comm/comm_module.hh"
#include "widgets/4ms/quantities.hh"

void CommModule::onSampleRateChange() {
	sampleRateChanged = true;
}

void CommModule::process(const ProcessArgs &args) {
	for (unsigned i = 0; auto &p : params) {

		// Transform VCV scaled values to MetaModule 0...1:
		// This is needed for 3-pos switches only
		auto val = getParamQuantity(i)->toScaled(p.getValue());

		core->set_param(i, val);
		i++;
	}

	for (auto &injack : inJacks) {
		auto id = injack.getId();
		injack.updateInput();

		if (injack.isJustPatched())
			core->mark_input_patched(id);

		if (injack.isJustUnpatched()) {
			core->set_input(id, 0);
			core->mark_input_unpatched(id);
		}

		if (injack.isConnected()) {
			auto scaledIn = injack.getValue();
			core->set_input(id, scaledIn);
		}
	}

	if (sampleRateChanged) {
		sampleRateChanged = false;
		core->set_samplerate(args.sampleRate);
	}

	// Patched state needs to be set before update
	// otherwise the first sample after patching can be undefined
	for (auto &out : outJacks) {

		auto id = out.getId();

		if (out.isConnected())
			core->mark_output_patched(id);
		else
			core->mark_output_unpatched(id);
	}

	core->update();

	// Always set output independent of patch state
	// since when unpatched the value will not be used anyway
	for (auto &out : outJacks) {

		auto raw_value = core->get_output(out.getId());
		out.setValue(raw_value);
	}

	for (unsigned i = 0; auto &light : lights) {
		light.setBrightness(core->get_led_brightness(i));
		i++;
	}

	// Reset alt param actions after module handles them
	for (auto i : alt_param_action_indices) {
		auto val = core->get_param(i);
		auto rackVal = getParamQuantity(i)->getScaledValue();
		if (val != rackVal) {
			getParamQuantity(i)->setScaledValue(val);
		}
	}
}

void CommModule::configComm(unsigned NUM_PARAMS, unsigned NUM_INPUTS, unsigned NUM_OUTPUTS, unsigned NUM_LIGHTS) {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

	for (unsigned i = 0; i < NUM_INPUTS; i++) {
		inJacks.push_back({inputs[i], i});
	}
	for (unsigned i = 0; i < NUM_OUTPUTS; i++) {
		outJacks.push_back({outputs[i], i});
	}
	core->mark_all_inputs_unpatched();
	core->mark_all_outputs_unpatched();

	// Find any AltParamAction, since we need to manually reset their values
	alt_param_action_indices.clear();
	for (auto i = 0u; auto const &element : info.elements) {
		if (std::holds_alternative<MetaModule::AltParamAction>(element)) {
			alt_param_action_indices.push_back(info.indices[i].param_idx);
		}
		i++;
	}
}

json_t *CommModule::dataToJson() {
	const auto state_string = core->save_state();

	if (state_string.size())
		return json_string(state_string.c_str());
	else
		return nullptr;
}

void CommModule::dataFromJson(json_t *rootJ) {
	if (auto state_str = json_string_value(rootJ); state_str) {
		core->load_state(state_str);
	}
}

void CommModule::fromJson(json_t *rootJ) {
	// Check plugin version
	json_t *versionJ = json_object_get(rootJ, "version");
	if (versionJ) {
		patch_version = json_string_value(versionJ);
	}

	rack::engine::Module::fromJson(rootJ);
}

void CommModule::paramsFromJson(json_t *rootJ) {
	size_t i;
	json_t *paramJ;
	json_array_foreach(rootJ, i, paramJ) {
		// Get paramId
		json_t *paramIdJ = json_object_get(paramJ, "id");
		// Legacy v0.6 to <v1
		if (!paramIdJ)
			paramIdJ = json_object_get(paramJ, "paramId");
		size_t paramId;
		if (paramIdJ)
			paramId = json_integer_value(paramIdJ);
		// Use index if all else fails
		else
			paramId = i;

		// Check ID bounds
		if (paramId >= paramQuantities.size())
			continue;

		rack::ParamQuantity *pq = paramQuantities[paramId];
		// Check that the Param is bounded
		if (!pq->isBounded())
			continue;

		if (json_t *valueJ = json_object_get(paramJ, "value")) {
			float val = json_number_value(valueJ);

			if (rack::string::Version(patch_version) < rack::string::Version("2.0.15")) {

				bool is_switch = dynamic_cast<rack::SwitchQuantity *>(pq) != nullptr;
				bool is_snapped_knob = dynamic_cast<KnobSnappedParamQuantity *>(pq) != nullptr;
				bool has_custom_range = (pq->minValue != 0 || pq->maxValue != 1);

				// Do not convert normal switches, but do convert snapped knobs (which are technically switches)
				if ((val <= 1 && val >= 0) && has_custom_range && (!is_switch || is_snapped_knob)) {
					// custom range: legacy versions saved 0..1 in patch file, so convert that
					val = pq->minValue * (1.f - val) + pq->maxValue * val;
				}
			}

			pq->setImmediateValue(val);
		}
	}
}
