#include "hub_map_button.hh"
#include "mapping/map_palette.hh"
#include <cstdio>

namespace MetaModule
{

void HubMapButton::draw(const DrawArgs &args) {
	hubParamObj.moduleID = parent.module ? parent.module->id : -1;

	if (!hub)
		return;

	bool isCurrentMapSrc = (hub->getMappingSource() == hubParamObj.objID);

	// Draw a large background circle to highlight a mapping has begun from this knob
	// Todo: Figure out a way to also draw the ring when hovering the knob it was mapped to.
	if (isCurrentMapSrc || hovered) {
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, box.size.x / 2, box.size.y / 2, box.size.y / 2);
		const float alpha = isCurrentMapSrc ? 0.75f : 0.4f;
		nvgFillColor(args.vg, rack::color::alpha(PaletteHub::color(hubParamObj.objID), alpha));
		nvgFill(args.vg);
	}

	// Draw the label text
	text = hub->mappings.getMapAliasName(hubParamObj);
	nvgBeginPath(args.vg);
	nvgTextAlign(args.vg, NVGalign::NVG_ALIGN_CENTER | NVGalign::NVG_ALIGN_MIDDLE);
	nvgFillColor(args.vg, nvgRGB(0xbb, 0xbb, 0xbb));
	nvgFontSize(args.vg, 9.0f);
	nvgText(args.vg, box.size.x / 2.0f, box.size.y + 10, text.c_str(), NULL);
}

void HubMapButton::onDragStart(const rack::event::DragStart &e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
		return;
	}

	start_mapping();

	// ???What is this?
	if (quantity)
		quantity->setMax();
}

void HubMapButton::start_mapping() {
	// Start a mapping unless one is in progress from this object (in which case, abort it)
	bool currentSourceIsThisButton = false;

	if (hub->isMappingInProgress()) {
		currentSourceIsThisButton = (hub->getMappingSource() == hubParamObj.objID);
		hub->endMapping();
	}
	if (!currentSourceIsThisButton) {
		APP->scene->rack->setTouchedParam(nullptr);
		hub->startMappingFrom(hubParamObj.objID);
	}
}

void HubMapButton::onDeselect(const rack::event::Deselect &e) {
	end_mapping();
}

void HubMapButton::end_mapping() {
	if (!hub)
		return;

	// Check if a ParamWidget was touched
	auto touchedParam = APP->scene->rack->getTouchedParam();

	if (touchedParam && touchedParam->getParamQuantity()) {
		int param_id = touchedParam->getParamQuantity()->paramId;
		auto m = touchedParam->module;

		APP->scene->rack->setTouchedParam(nullptr);

		if (m->getModel()->slug == "MMButtonExpander" || m->getModel()->slug == "HubMedium") {
			hub->endMapping();
		} else {
			hub->registerMap(hubParamObj.objID, m, param_id);
		}

	} else {
		hub->endMapping();
	}
}

void HubMapButton::onHover(const rack::event::Hover &e) {
	static unsigned flash = 0;
	constexpr unsigned flash_rate = 6;
	if (hub) {
		auto &maps = hub->mappings.getAllMappings(hubParamObj.objID);
		for (auto &map : maps) {
			map.paramHandle.color = (flash < flash_rate / 2) ? PaletteHub::color(hubParamObj.objID) :
															   PaletteHub::flash_color(hubParamObj.objID);
		}
		flash = flash ? flash - 1 : flash_rate;
	}
	e.consume(this);
}

void HubMapButton::onLeave(const rack::event::Leave &e) {
	hovered = false;
	if (hub) {
		auto &maps = hub->mappings.getAllMappings(hubParamObj.objID);
		for (auto &map : maps) {
			map.paramHandle.color = PaletteHub::color(hubParamObj.objID);
		}
	}
	e.consume(this);
}

void HubMapButton::onEnter(const rack::event::Enter &e) {
	hovered = true;
	if (hub) {
		auto &maps = hub->mappings.getAllMappings(hubParamObj.objID);
		for (auto &map : maps) {
			map.paramHandle.color = PaletteHub::WHITE;
		}
	}
	e.consume(this);
}

} // namespace MetaModule
