#pragma once
#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/elements/elements.hh"
#include "console/pr_dbg.hh"
#include "hub/hub_module_widget.hh"
#include "hub/jack_alias_menu.hh"
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

struct HubPort : rack::PJ301MPort {
	std::string *alias;

	void appendContextMenu(rack::ui::Menu *menu) override {
		using namespace rack;
		menu->addChild(new MenuSeparator());
		menu->addChild(new JackNameMenuItem{[this](unsigned _, std::string const &text) { *alias = text; },
											{},
											"alias",
											*alias,
											MetaModuleHubWidget::kMaxJackAliasChars});
		menu->addChild(new MenuSeparator());
	}
};

inline void do_create(JackInput el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	auto p = rack::createInputCentered<HubPort>(rack::mm2px({el.x_mm, el.y_mm}), ctx.module, idx.input_idx);
	p->alias = &ctx.module_widget->hubModule->jack_alias.out[idx.input_idx];
	ctx.module_widget->addInput(p);
}

inline void do_create(JackOutput el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	auto p = rack::createOutputCentered<HubPort>(rack::mm2px({el.x_mm, el.y_mm}), ctx.module, idx.output_idx);
	p->alias = &ctx.module_widget->hubModule->jack_alias.in[idx.output_idx];
	ctx.module_widget->addOutput(p);
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
