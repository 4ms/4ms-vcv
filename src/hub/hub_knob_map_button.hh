#pragma once
#include "hub/hub_knob_menu.hh"
#include "hub_knob_menu.hh"
#include "hub_map_button.hh"
#include "hub_module.hh"
#include "mapping/knob_alias_menu.hh"
#include "mapping/range_slider.hh"

class HubKnobMapButton : public HubMapButton {
	rack::ParamQuantity *paramQuantity = nullptr;
	MetaModuleHubBase *hub;

public:
	HubKnobMapButton(MetaModuleHubBase *hub, rack::app::ModuleWidget &parent)
		: HubMapButton{hub, parent}
		, hub{hub} {
	}

	void setParamQuantity(rack::ParamQuantity *paramQ) {
		paramQuantity = paramQ;
	}

	void onDeselect(const rack::event::Deselect &e) override {
		end_mapping();
	}

	void end_mapping() {
		if (!hub)
			return;

		// Check if a ParamWidget was touched
		auto touchedParam = APP->scene->rack->getTouchedParam();

		if (touchedParam && touchedParam->getParamQuantity()) {
			int param_id = touchedParam->getParamQuantity()->paramId;
			auto m = touchedParam->module;

			APP->scene->rack->setTouchedParam(nullptr);
			hub->registerMap(hubParamObj.objID, m, param_id);

		} else {
			hub->endMapping();
		}
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
			for (auto const &knob : knob_sets) {
				auto &ph = knob.paramHandle;
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

				auto *sep = new rack::MenuSeparator;
				menu->addChild(sep);

				MappedKnobMenuLabel *paramLabel2 = new MappedKnobMenuLabel;
				paramLabel2->moduleName = ph.module->model->name;
				paramLabel2->paramName = ph.module->paramQuantities[paramId]->getLabel();
				paramLabel2->moduleId = moduleId;
				paramLabel2->paramId = paramId;
				menu->addChild(paramLabel2);

				MappableObj paramObj{MappableObj::Type::Knob, paramId, moduleId};

				auto mn = new RangeSlider<RangePart::Min>(hub, paramObj);
				mn->box.size.x = 100;
				menu->addChild(mn);

				auto mx = new RangeSlider<RangePart::Max>(hub, paramObj);
				mx->box.size.x = 100;
				menu->addChild(mx);
			}

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
		rack::ParamQuantity *paramQuantity;

		void onAction(const rack::event::Action &e) override {
			rack::ParamHandle *paramHandle =
				APP->engine->getParamHandle(paramQuantity->module->id, paramQuantity->paramId);
			if (paramHandle) {
				APP->engine->updateParamHandle(paramHandle, -1, 0);
			}
		}
	};
};
