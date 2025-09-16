#pragma once
#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/elements/elements.hh"
#include "console/pr_dbg.hh"
#include "hub/hub_jack_label.hh"
#include "hub/hub_module_widget.hh"
#include "hub/jack_alias_menu.hh"
#include "widgets/4ms/4ms_widgets.hh"
#include <rack.hpp>

namespace MetaModule
{

struct HubWidgetContext {
	MetaModuleHubWidget *module_widget;
	MetaModuleHubBase *module;
};

namespace HubWidgetImpl
{

inline void do_create(BaseElement element, const ElementCount::Indices &, const HubWidgetContext &) {
	pr_dbg("Hub Widget not found\n");
}

inline void do_create(Knob el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	auto ctr_pos = rack::mm2px({el.x_mm, el.y_mm});
	if (el.image.ends_with("knob_x.png"))
		ctx.module_widget->addHubMappedParam<Davies1900hBlackKnob4ms>(
			el.short_name, idx.param_idx, ctr_pos, MappableObj::Type::Knob, 19.f);
	else
		ctx.module_widget->addHubMappedParam<Small9mmKnob>(
			el.short_name, idx.param_idx, ctr_pos, MappableObj::Type::Knob, 14.f);
}

struct HubPort : rack::PJ301MPort {
	std::string *alias;

	void appendContextMenu(rack::ui::Menu *menu) override {
		using namespace rack;
		menu->addChild(new MenuSeparator());
		auto label = new MenuLabel;
		label->text = "Alias:";
		menu->addChild(label);
		menu->addChild(new JackNameMenuItem{[this](unsigned, std::string const &text) { *alias = text; },
											"alias",
											*alias,
											MetaModuleHubWidget::kMaxJackAliasChars});
		menu->addChild(new MenuSeparator());
	}
};

inline void do_create(MomentaryButton el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	auto ctr_pos = rack::mm2px({el.x_mm, el.y_mm});
	ctx.module_widget->addHubMappedParam<rack::VCVButton>(
		el.short_name, idx.param_idx, ctr_pos, MappableObj::Type::Button, 15.f);
}

struct HubPort : rack::PJ301MPort {
	std::string *alias;

	void appendContextMenu(rack::ui::Menu *menu) override {
		using namespace rack;
		menu->addChild(new MenuSeparator());
		auto label = new MenuLabel;
		label->text = "Alias:";
		menu->addChild(label);
		menu->addChild(new JackNameMenuItem{[this](unsigned, std::string const &text) { *alias = text; },
											"alias",
											*alias,
											MetaModuleHubWidget::kMaxJackAliasChars});
		menu->addChild(new MenuSeparator());
	}
};

inline void do_create(JackInput el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	// These are Input jacks as seen by VCV, but they are called "Out 1", "Out 2", etc
	auto p = rack::createInputCentered<HubPort>(rack::mm2px({el.x_mm, el.y_mm}), ctx.module, idx.input_idx);
	if (ctx.module_widget->hubModule) {
		p->alias = &ctx.module_widget->hubModule->jack_alias.out[idx.input_idx];
	}
	ctx.module_widget->addInput(p);

	auto label = new HubJackLabel{ctx.module, idx.input_idx, HubJackLabel::JackDir::Input};
	label->box.size = rack::mm2px({14, 4});
	label->box.pos = rack::mm2px({el.x_mm, el.y_mm + 7.5f}) - label->box.size.div(2);
	ctx.module_widget->addChild(label);
}

inline void do_create(JackOutput el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	// These are Output jacks as seen by VCV, but they are called "In 1", "In 2", etc
	auto p = rack::createOutputCentered<HubPort>(rack::mm2px({el.x_mm, el.y_mm}), ctx.module, idx.output_idx);
	if (ctx.module_widget->hubModule) {
		p->alias = &ctx.module_widget->hubModule->jack_alias.in[idx.output_idx];
	}
	ctx.module_widget->addOutput(p);

	auto label = new HubJackLabel{ctx.module, idx.output_idx, HubJackLabel::JackDir::Output};
	label->box.size = rack::mm2px({14, 4});
	label->box.pos = rack::mm2px({el.x_mm, el.y_mm + 7.5f}) - label->box.size.div(2);
	ctx.module_widget->addChild(label);
}

inline void do_create(LatchingButton el, const ElementCount::Indices &idx, const HubWidgetContext &ctx) {
	ctx.module_widget->addParam(
		rack::createParamCentered<rack::BefacoPush>(rack::mm2px({el.x_mm, el.y_mm}), ctx.module, idx.param_idx));
}

} // namespace HubWidgetImpl

template<typename INFO>
struct HubWidgetCreator {

	HubWidgetCreator(MetaModuleHubWidget *module_widget, MetaModuleHubBase *module)
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
