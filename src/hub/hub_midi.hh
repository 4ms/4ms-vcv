#pragma once
#include "hub_knob.hh"

class HubMidiMapButton : public HubKnobMapButton {
	MetaModuleHubBase *hub;

public:
	HubMidiMapButton(MetaModuleHubBase *hub, rack::app::ModuleWidget &parent)
		: HubKnobMapButton{hub, parent}
		, hub{hub} {
	}

	void draw(const DrawArgs &args) override {
		if (!hub)
			return;

		bool isCurrentMapSrc = (hub->getMappingSource() == hubParamObj.objID);

		// Draw a large background rounded rect to highlight a mapping has begun from this knob
		// Same as HubMapButton::draw except use a rounded rect, and don't draw alias below
		if (isCurrentMapSrc || hovered) {
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 4);
			const float alpha = isCurrentMapSrc ? 0.75f : 0.4f;
			nvgFillColor(args.vg, rack::color::alpha(PaletteHub::color(hubParamObj.objID), alpha));
			nvgFill(args.vg);
		}
	}

	//TODO:
	//
	// void onDragStart(const rack::event::DragStart &e) override
	// {
	// 	if (id.objType == MappableObj::Type::MidiNote) {
	// 	}
	// }
};

class HubMidiParam : public rack::ParamWidget {
public:
	HubMidiParam(MetaModuleHubBase *hub, HubMidiMapButton &mapbut)
		: hub{hub}
		, mapBut{mapbut} {
	}

	void draw(const DrawArgs &args) override {
		if (!hub)
			return;

		auto numMaps = std::min(hub->mappings.getNumActiveMappings(mapBut.hubParamObj.objID), 16U);

		// TODO: different color for each one
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
			if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0) {
				if (this->getParamQuantity()) {
					APP->scene->rack->setTouchedParam(this);
				}
				e.consume(this);
			}

			// Right click to open context menu
			if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT && (e.mods & RACK_MOD_MASK) == 0) {
				mapBut.makeKnobMenu(); //this->getParamQuantity(), hubmidi_mapbut.mapObj);
				e.consume(this);
			}
		}
	}

	void onHover(const rack::event::Hover &e) override {
		// For MidiMap, always show the highlight. Returning allows HubMidiMapButton to handle onHover
		return;
	}

	// onDeselect and onDragStart are passed to the HubMidiMapButton
	// In order to make the HubMidiParam seem "invisible"
	// but still will accept onButton events so it can be mapped
	// to VCV's MidiMaps module
	void onDeselect(const rack::event::Deselect &e) override {
		rack::ParamWidget::onDeselect(e);
		mapBut.onDeselect(e);
	}

	void onDragStart(const rack::event::DragStart &e) override {
		mapBut.onDragStart(e);
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
