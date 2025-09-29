#pragma once
#include "hub/hub_knob_menu.hh"
#include "hub_knob_menu.hh"
#include "hub_map_button.hh"
#include "hub_module.hh"
#include "mapping/knob_alias_menu.hh"
#include "mapping/range_slider.hh"

namespace MetaModule
{

class HubKnobMapButton : public HubMapButton {
public:
	rack::ParamQuantity *paramQuantity = nullptr;

	HubKnobMapButton(MetaModuleHubBase *hub, rack::app::ModuleWidget &parent)
		: HubMapButton{hub, parent} {
	}

	void onButton(const rack::event::Button &e) override {
		// Right click to open context menu
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT && (e.mods & RACK_MOD_MASK) == 0) {
			if (paramQuantity) {
				makeKnobMenu();
				e.consume(this);
			}
		} else {
			Button::onButton(e);
		}
	}

	void makeKnobMenu() {
		rack::ui::Menu *menu = rack::createMenu();

		KnobNameMenuLabel *paramLabel = new KnobNameMenuLabel;
		paramLabel->paramQty = paramQuantity;
		menu->addChild(paramLabel);

		KnobValueMenuItem *paramField = new KnobValueMenuItem{120, 0.4f, paramQuantity};
		menu->addChild(paramField);

		if (!hub)
			return;

		if (hub->mappings.getNumActiveMappings(hubParamObj.objID) > 0) {
			auto *sep = new rack::MenuSeparator;
			menu->addChild(sep);

			auto aliasItem = new KnobAliasMenuItem{hub, hubParamObj};
			menu->addChild(aliasItem);

			auto &knob_sets = hub->mappings.getAllMappings(hubParamObj.objID);
			for (auto const &mapset : knob_sets) {
				auto &ph = mapset.paramHandle;
				if (!ph.module)
					continue;
				if (ph.module->id < 0)
					continue;
				if (!ph.module->model)
					continue;
				auto paramId = ph.paramId;
				auto moduleId = ph.moduleId;
				if (!ph.module->paramQuantities[paramId])
					continue;

				menu->addChild(new rack::MenuSeparator);

				MappedKnobMenuLabel *paramLabel2 = new MappedKnobMenuLabel;
				paramLabel2->moduleName = ph.module->model->name;
				paramLabel2->paramName = ph.module->paramQuantities[paramId]->getLabel();
				paramLabel2->moduleId = moduleId;
				paramLabel2->paramId = paramId;
				menu->addChild(paramLabel2);

				ParamUnmapItem *unmapItem = new ParamUnmapItem;
				unmapItem->text = "Unmap";
				unmapItem->paramQuantity = ph.module->paramQuantities[paramId];
				unmapItem->unmapAction = [this] {
					if (hub->mappings.getNumActiveMappings(hubParamObj.objID) == 0)
						hub->mappings.setMapAliasName(hubParamObj.objID, "");
				};
				menu->addChild(unmapItem);

				MappableObj paramObj{MappableObj::Type::Knob, paramId, moduleId};

				menu->addChild(new rack::MenuSeparator);

				if (hubParamObj.objType == MappableObj::Type::Button) {
					menu->addChild(rack::createBoolMenuItem(
						"Button toggles parameter",
						"",
						[=, this]() { return hub->mappings.getCurveType(paramObj); },
						[=, this](bool val) { hub->mappings.setCurveType(paramObj, val ? 1 : 0); }));
					menu->addChild(new rack::MenuSeparator);
				}

				auto mn = new RangeSlider<RangePart::Min>(hub, paramObj);
				mn->box.size.x = 100;
				menu->addChild(mn);

				auto mx = new RangeSlider<RangePart::Max>(hub, paramObj);
				mx->box.size.x = 100;
				menu->addChild(mx);
			}

			// Add "Unmap" item if the Hub knob is mapped to another module (e.g. MIDI CC module)
			rack::ParamHandle *paramHandle =
				paramQuantity ? APP->engine->getParamHandle(paramQuantity->module->id, paramQuantity->paramId) :
								nullptr;
			if (paramHandle) {
				ParamUnmapItem *unmapItem = new ParamUnmapItem;
				unmapItem->text = "Unmap";
				unmapItem->rightText = paramHandle->text;
				unmapItem->paramQuantity = paramQuantity;
				menu->addChild(unmapItem);
			}
		}
	}

	struct ParamUnmapItem : rack::ui::MenuItem {
		rack::ParamQuantity *paramQuantity = nullptr;
		std::function<void(void)> unmapAction;

		void onAction(const rack::event::Action &e) override {
			if (paramQuantity && paramQuantity->module) {
				auto *paramHandle = APP->engine->getParamHandle(paramQuantity->module->id, paramQuantity->paramId);

				if (paramHandle) {
					APP->engine->updateParamHandle(paramHandle, -1, 0);
				}
			}

			if (unmapAction) {
				unmapAction();
			}
		}
	};
};

} // namespace MetaModule
