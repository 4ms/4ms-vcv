#pragma once
#include "hub/knob_set_buttons.hh"
#include "hub/knob_set_menu.hh"
#include "hub/text_field.hh"
#include "hub_knob.hh"
#include "hub_module.hh"
#include <functional>
#include <string>

namespace MetaModule
{

struct MetaModuleHubWidget : rack::app::ModuleWidget {
	MetaModuleHubWidget() = default;

	MetaModuleHubBase *hubModule;

	rack::Label *statusText;
	KnobSetButtonGroup *knobSetButtons;
	TextField *knobSetNameField;

	static constexpr float kKnobSpacingY = 17;
	static constexpr float kKnobSpacingX = 18;
	static constexpr float kTextOffset = 5;

	static constexpr unsigned kMaxKnobSetNameChars = 16;

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

	void appendContextMenu(rack::Menu *menu) override {
		using namespace rack;
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel<MenuLabel>("Mapped Knob Sets"));

		for (unsigned knobset_idx = 0; knobset_idx < hubModule->MaxKnobSets; knobset_idx++) {
			menu->addChild(new MenuSeparator());
			menu->addChild(createCheckMenuItem(
				string::f("Knob Set %d", knobset_idx + 1),
				"",
				[=, this]() { return hubModule->mappings.getActiveKnobSetIdx() == knobset_idx; },
				[=, this]() {
					hubModule->mappings.changeActiveKnobSet(knobset_idx, ShouldLock::Yes);
					updateKnobSetLabel();
				}));

			menu->addChild(new KnobSetNameMenuItem{[this](unsigned idx, std::string const &text) {
													   hubModule->mappings.setKnobSetName(idx, text);
													   updateKnobSetLabel();
												   },
												   knobset_idx,
												   hubModule->mappings.getKnobSetName(knobset_idx),
												   kMaxKnobSetNameChars});
		}
	}

	void updateKnobSetLabel() {
		if (!hubModule || !knobSetNameField || !knobSetButtons)
			return;

		auto activeKnobSetIdx = hubModule->mappings.getActiveKnobSetIdx();
		if (auto name = hubModule->mappings.getActiveKnobSetName(); name.length() > 0) {
			knobSetNameField->text = name;
		} else {
			knobSetNameField->text = rack::string::f("Knob Set %d", activeKnobSetIdx + 1);
		}

		knobSetButtons->active_idx = activeKnobSetIdx;
	}
};

} // namespace MetaModule
