#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/elements/elements.hh"
#include "vcv_creation_context.hh"

namespace MetaModule
{

using Indices = ElementCount::Indices;

inline void do_config_element(JackInput el, const Indices &indices, const ModuleContext_t &context) {
	std::string_view name = el.short_name;
	if (name.ends_with(" In"))
		name.remove_suffix(3);
	else if (name.ends_with(" in"))
		name.remove_suffix(3);
	else if (name.ends_with(" Input"))
		name.remove_suffix(6);
	else if (name.ends_with(" input"))
		name.remove_suffix(6);
	else if (name.ends_with("In"))
		name.remove_suffix(2);
	else if (name.ends_with("Input"))
		name.remove_suffix(5);

	context.module->configInput(indices.input_idx, std::string{name});
};

inline void do_config_element(JackOutput el, const Indices &indices, const ModuleContext_t &context) {
	std::string_view name = el.short_name;
	if (name.ends_with(" Out"))
		name.remove_suffix(4);
	else if (name.ends_with(" out"))
		name.remove_suffix(4);
	else if (name.ends_with(" Output"))
		name.remove_suffix(7);
	else if (name.ends_with(" output"))
		name.remove_suffix(7);
	else if (name.ends_with("Out"))
		name.remove_suffix(3);
	else if (name.ends_with("Output"))
		name.remove_suffix(6);

	context.module->configOutput(indices.output_idx, std::string{name});
};

inline void do_config_element(Pot el, const Indices &indices, const ModuleContext_t &context) {
	float display_mult = el.display_mult;
	std::string units{el.units};

	if (el.min_value == 0 && el.max_value == 1 && el.units == "" && el.display_mult == 1) {
		display_mult = 100;
		units = "%";
	}

	el.default_value = el.default_value * (el.max_value - el.min_value) + el.min_value;

	context.module->configParam(indices.param_idx,
								el.min_value,
								el.max_value,
								el.default_value,
								std::string{el.short_name},
								units,
								el.display_base,
								display_mult,
								el.display_offset);
};

inline void do_config_element(LightElement el, const Indices &indices, const ModuleContext_t &context) {
	context.module->configLight(indices.light_idx, el.short_name.data());
};

inline void do_config_element(MomentaryButton el, const Indices &indices, const ModuleContext_t &context) {
	context.module->configParam(indices.param_idx, 0, 1, 0, el.short_name.data());
}

inline void do_config_element(LatchingButton el, const Indices &indices, const ModuleContext_t &context) {
	float defaultValue = (el.default_value == LatchingButton::State_t::UP) ? 0 : 1;
	context.module->configParam(indices.param_idx, 0, 1, defaultValue, el.short_name.data());
}

inline void do_config_element(SlideSwitch el, const Indices &indices, const ModuleContext_t &context) {
	context.module->configParam(indices.param_idx, 0, el.num_pos - 1, el.default_value, el.short_name.data());
};

inline void do_config_element(FlipSwitch el, const Indices &indices, const ModuleContext_t &context) {
	std::vector<std::string> labels;
	for (auto p : el.pos_names)
		labels.push_back(std::string{p});
	context.module->configSwitch(indices.param_idx, 0, el.num_pos - 1, el.default_value, el.short_name.data(), labels);
};

inline void do_config_element(Encoder el, const Indices &indices, const ModuleContext_t &context) {
	context.module->configParam(indices.param_idx, -INFINITY, INFINITY, 0, el.short_name.data());
};

inline void do_config_element(AltParamContinuous el, const Indices &indices, const ModuleContext_t &context) {
	context.module->configParam(indices.param_idx, el.MinValue, el.MaxValue, el.default_value, el.short_name.data());
}

inline void do_config_element(AltParamChoice el, const Indices &indices, const ModuleContext_t &context) {
	context.module->configParam(indices.param_idx, 0, el.num_pos - 1, el.default_value, el.short_name.data());
}

} // namespace MetaModule
