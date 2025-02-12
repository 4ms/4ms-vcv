#pragma once
#include "hub/jack_alias_menu.hh"
#include "hub/knob_set_buttons.hh"
#include "hub/knob_set_menu.hh"
#include "hub/text_field.hh"
#include "hub_knob.hh"
#include "hub_module.hh"
#include <string_view>

namespace MetaModule
{

struct MetaModuleHubWidget : rack::app::ModuleWidget {
	MetaModuleHubWidget() = default;

	MetaModuleHubBase *hubModule;

	static constexpr float kKnobSpacingX = 18;
	static constexpr unsigned kMaxJackAliasChars = 16;

	template<typename KnobType>
	void addLabeledKnobPx(std::string_view labelText,
						  int knobId,
						  rack::math::Vec posPx,
						  float sz_mm = kKnobSpacingX,
						  float defaultValue = 0.5f) {
		auto button = new HubKnobMapButton{hubModule, *this};
		button->box.pos =
			rack::math::Vec(posPx.x - rack::mm2px(sz_mm) / 2, posPx.y - rack::mm2px(sz_mm) / 2); // top-left
		button->box.size.x = rack::mm2px(sz_mm);
		button->box.size.y = rack::mm2px(sz_mm);
		button->text = labelText;
		button->hubParamObj = {MappableObj::Type::Knob, knobId, hubModule ? hubModule->id : -1};
		addChild(button);

		auto *p = new HubKnob<KnobType>{hubModule, *button};
		p->box.pos = posPx;
		p->box.pos = p->box.pos.minus(p->box.size.div(2));
		p->rack::app::ParamWidget::module = hubModule;
		p->rack::app::ParamWidget::paramId = knobId;
		p->initParamQuantity();

		if (module) {
			auto pq = p->getParamQuantity();
			pq = module->paramQuantities[knobId];
			pq->defaultValue = defaultValue;
			button->setParamQuantity(pq);
		}
		addParam(p);
	}

protected:
	static rack::ui::MenuItem *createAliasSubmenu(std::span<std::tuple<std::span<std::string>, const char *>> jacks_) {
		using namespace rack;

		std::vector<std::remove_reference_t<decltype(jacks_[0])>> jacks;
		jacks.insert(jacks.begin(), jacks_.begin(), jacks_.end());

		return createSubmenuItem("Jack Aliases", "", [jacks](Menu *menu) {
			for (auto &j : jacks) {
				menu->addChild(new MenuSeparator());
				menu->addChild(createMenuLabel<MenuLabel>(std::get<1>(j)));
				for (auto jack_idx = 0u; jack_idx < std::get<0>(j).size(); jack_idx++) {
					menu->addChild(new MenuSeparator());
					menu->addChild(
						new JackNameMenuItem{[j](unsigned idx, std::string const &text) { std::get<0>(j)[idx] = text; },
											 jack_idx,
											 std::get<1>(j) + std::to_string(jack_idx + 1),
											 std::get<0>(j)[jack_idx],
											 kMaxJackAliasChars});
				}
			}
		});
	}
};

} // namespace MetaModule
