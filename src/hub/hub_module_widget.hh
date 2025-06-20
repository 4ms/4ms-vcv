#pragma once
#include "hub/knob_set_buttons.hh"
#include "hub/knob_set_menu.hh"
#include "hub/text_field.hh"
#include "hub_module.hh"
#include "hub_param.hh"
#include <string_view>

namespace MetaModule
{

struct MetaModuleHubWidget : rack::app::ModuleWidget {
	MetaModuleHubBase *hubModule;

	MetaModuleHubWidget(MetaModuleHubBase *hubModule)
		: hubModule{hubModule} {
	}

	template<typename KnobType, typename HubMapButtonType = HubKnobMapButton>
	void addHubMappedParam(
		std::string_view labelText, int knobId, rack::math::Vec posPx, float sz_mm = 18, float defaultValue = 0.5f) {
		auto button = new HubMapButtonType{hubModule, *this};
		button->box.pos =
			rack::math::Vec(posPx.x - rack::mm2px(sz_mm) / 2, posPx.y - rack::mm2px(sz_mm) / 2); // top-left
		button->box.size.x = rack::mm2px(sz_mm);
		button->box.size.y = rack::mm2px(sz_mm);
		button->text = labelText;
		button->hubParamObj = {MappableObj::Type::Knob, knobId, hubModule ? hubModule->id : -1};
		addChild(button);

		auto *p = new HubParam<KnobType>{hubModule, *button};
		p->box.pos = posPx;
		p->box.pos = p->box.pos.minus(p->box.size.div(2));
		p->rack::app::ParamWidget::module = hubModule;
		p->rack::app::ParamWidget::paramId = knobId;
		p->initParamQuantity();

		if (module) {
			auto pq = p->getParamQuantity();
			pq = module->paramQuantities[knobId];
			pq->defaultValue = defaultValue;
			button->paramQuantity = pq;
		}
		addParam(p);
	}
};

} // namespace MetaModule
