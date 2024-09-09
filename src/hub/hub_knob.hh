#pragma once
#include "hub_knob_map_button.hh"
#include "mapping/map_marks.hh"
#include "mapping/map_palette.hh"

namespace MetaModule
{

template<typename BaseKnobT>
class HubKnob : public BaseKnobT {
public:
	HubKnob(MetaModuleHubBase *hub, HubKnobMapButton &hubknob_mapbut)
		: hub{hub}
		, mapBut{hubknob_mapbut} {
	}

	void draw(const typename BaseKnobT::DrawArgs &args) override {
		BaseKnobT::draw(args);

		if (!hub)
			return;

		auto numMaps = std::min(hub->mappings.getNumActiveMappings(mapBut.hubParamObj.objID), 16U);

		const float spacing = 8;
		const NVGcolor color = PaletteHub::color(mapBut.hubParamObj.objID);
		auto _box = this->box;
		for (unsigned i = 0; i < numMaps; i++) {
			MapMark::markKnob(args.vg, _box, color);
			if (i % 4 == 3) {
				_box.size.x = this->box.size.x;
				_box.size.y -= spacing;
			} else
				_box.size.x -= spacing;
		}
	}

	void onButton(const rack::event::Button &e) override {
		rack::math::Vec c = this->box.size.div(2);
		float dist = e.pos.minus(c).norm();
		if (dist <= c.x) {
			rack::OpaqueWidget::onButton(e);

			// Touch parameter
			// This is needed in case someone maps a Hub Knobs to their MIDI CC module or something else
			if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
				if ((e.mods & RACK_MOD_MASK) == 0) {
					if (this->getParamQuantity()) {
						APP->scene->rack->setTouchedParam(this);
					}
					e.consume(this);
				} else if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {
					mapBut.start_mapping();
				}
			}

			// Right click to open context menu
			if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT && (e.mods & RACK_MOD_MASK) == 0) {
				mapBut.makeKnobMenu();
				e.consume(this);
			}
		}
	}

	void onDeselect(const rack::event::Deselect &e) override {
		mapBut.end_mapping();
	}

	void onHover(const rack::event::Hover &e) override {
		// If the knob is mapped, then we want to pass the hover down to the HubKnobMapButton object below
		// so that the HubMapKnobButton can highlight even if we're hovering the knob itself.
		// So, don't consume the hover and just do nothing.
		// On the other hand, if the knob is not mapped, then consume the hover so that hovering the knob
		// doesn't make the background highlight appear
		// if (hub) {
		// 	if (hub->mappings.getNumMappings(mapBut.hubParamObj.objID) > 0)
		// 		return;
		// }

		e.consume(this);
	}

	struct ParamResetItem : rack::ui::MenuItem {
		rack::ParamWidget *paramWidget;
		void onAction(const rack::event::Action &e) override {
			paramWidget->resetAction();
		}
	};

private:
	MetaModuleHubBase *hub;
	HubKnobMapButton &mapBut;
};

} // namespace MetaModule
