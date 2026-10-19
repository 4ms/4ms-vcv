#pragma once
#include "CoreModules/elements/units.hh"
#include "comm/comm_module.hh"
#include "hub/hub_screensaver.hh"
#include "widgets/4ms/4ms_widgets.hh"
#include <rack.hpp>
#include <string>
#include <vector>

namespace MetaModule
{

struct ScreensaverDisplayChoice {
	std::string name;
	HubScreensaverDisplay display;
};

// The displays MetaModule could show full-screen for a module.
//
// 4ms modules draw each display with a GraphicDisplayWidget, which knows its light index.
// For other modules, MetaModule treats a ModuleWidget child as a graphic display if it isn't
// a param, port, coloured light, screw, SVG image/button, or the panel
// (see metamodule firmware/vcv_plugin/export/src/app/ModuleWidget.cpp). It identifies them by
// the top-left position, as MetaModule does: to_mm(widget->box.pos).
inline std::vector<ScreensaverDisplayChoice> findScreensaverDisplays(rack::app::ModuleWidget *mw) {
	using namespace rack;

	std::vector<ScreensaverDisplayChoice> choices;
	if (!mw || !mw->module)
		return choices;

	auto moduleId = mw->module->getId();

	// MetaModule doesn't show tiny displays full-screen
	auto big_enough = [](widget::Widget const *w) {
		return to_mm(w->box.size.x) >= 5 || to_mm(w->box.size.y) >= 5;
	};

	if (auto *comm = dynamic_cast<CommModule *>(mw->module)) {
		for (auto *child : mw->children) {
			auto *display = dynamic_cast<GraphicDisplayWidget *>(child);
			if (!display || !big_enough(display))
				continue;

			std::string name = "Display";
			for (auto i = 0u; i < comm->info.elements.size() && i < comm->info.indices.size(); i++) {
				if (comm->info.indices[i].light_idx == display->display_idx) {
					name = base_element(comm->info.elements[i]).short_name;
					break;
				}
			}

			choices.push_back({name, {.moduleId = moduleId, .lightId = (uint16_t)display->display_idx}});
		}
		return choices;
	}

	unsigned num = 1;
	for (auto *child : mw->children) {
		if (child == mw->getPanel())
			continue;

		if (dynamic_cast<app::ParamWidget *>(child) || dynamic_cast<app::PortWidget *>(child) ||
			dynamic_cast<app::SvgScrew *>(child) || dynamic_cast<widget::SvgWidget *>(child) ||
			dynamic_cast<app::SvgButton *>(child) || dynamic_cast<app::SvgPanel *>(child))
			continue;

		if (auto *light = dynamic_cast<app::ModuleLightWidget *>(child); light && light->getNumColors())
			continue;

		if (!big_enough(child))
			continue;

		choices.push_back({"Display " + std::to_string(num++),
						   {.moduleId = moduleId, .x_mm = to_mm(child->box.pos.x), .y_mm = to_mm(child->box.pos.y)}});
	}

	return choices;
}

} // namespace MetaModule
