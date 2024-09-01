#pragma once
#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/elements/elements.hh"
#include "console/pr_dbg.hh"
#include "hub/hub_module_widget.hh"
#include "widgets/4ms/4ms_widgets.hh"

namespace MetaModule
{

struct HubWidgetContext {
	MetaModuleHubWidget *module_widget;
	rack::Module *module;
};

namespace HubWidgetImpl
{

inline void do_create(BaseElement element, const ElementCount::Indices &, const HubWidgetContext &) {
	pr_dbg("Hub Widget not found\n");
}

inline void do_create(Knob el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	auto ctr_pos = rack::mm2px({el.x_mm, el.y_mm});
	if (el.image.ends_with("knob_x.png"))
		ctx.module_widget->addLabeledKnobPx<Davies1900hBlackKnob4ms>(el.short_name, idx.param_idx, ctr_pos, 19.f);
	else
		ctx.module_widget->addLabeledKnobPx<Small9mmKnob>(el.short_name, idx.param_idx, ctr_pos, 14.f);
}

inline void do_create(JackInput el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	ctx.module_widget->addInput(
		rack::createInputCentered<rack::PJ301MPort>(rack::mm2px({el.x_mm, el.y_mm}), ctx.module, idx.input_idx));
}

inline void do_create(JackOutput el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	ctx.module_widget->addOutput(
		rack::createOutputCentered<rack::PJ301MPort>(rack::mm2px({el.x_mm, el.y_mm}), ctx.module, idx.output_idx));
}

inline void do_create(LatchingButton el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	ctx.module_widget->addParam(
		rack::createParamCentered<rack::BefacoPush>(rack::mm2px({el.x_mm, el.y_mm}), ctx.module, idx.param_idx));
}

} // namespace HubWidgetImpl

template<typename INFO>
struct HubWidgetCreator {

	HubWidgetCreator(MetaModuleHubWidget *module_widget, rack::Module *module)
		: context{module_widget, module} {
	}

	template<typename T>
	void create(const T element) {
		// forward to implementation together with current context
		if (auto indices = ElementCount::get_indices<INFO>(element)) {
			HubWidgetImpl::do_create(element, indices.value(), context);
		}
	}

private:
	HubWidgetContext context;
};

} // namespace MetaModule
