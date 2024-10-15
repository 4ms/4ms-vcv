#include "comm/comm_module.hh"

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
