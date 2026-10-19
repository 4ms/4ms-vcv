#pragma once
#include "CoreModules/elements/units.hh"
#include "comm/comm_module.hh"
#include "hub/hub_screensaver.hh"
#include "hub/screensaver_names.hh"
#include "widgets/4ms/4ms_widgets.hh"
#include <cstdlib>
#include <cxxabi.h>
#include <functional>
#include <rack.hpp>
#include <string>
#include <vector>

namespace MetaModule
{

struct ScreensaverDisplayHighlight;

struct ScreensaverDisplayChoice {
	std::string name;
	HubScreensaverDisplay display;
	rack::widget::Widget *widget;
};

inline bool isHighlight(rack::widget::Widget *w);

inline std::string demangledClassName(rack::widget::Widget *w) {
	const char *mangled = typeid(*w).name();
	int status = 0;
	char *demangled = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
	std::string name = (status == 0 && demangled) ? demangled : mangled;
	std::free(demangled);
	return name;
}

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

			choices.push_back({name, {.moduleId = moduleId, .lightId = (uint16_t)display->display_idx}, display});
		}
		return choices;
	}

	std::vector<std::string> class_names;
	for (auto *child : mw->children) {
		if (child == mw->getPanel())
			continue;

		if (dynamic_cast<app::ParamWidget *>(child) || dynamic_cast<app::PortWidget *>(child) ||
			dynamic_cast<app::SvgScrew *>(child) || dynamic_cast<widget::SvgWidget *>(child) ||
			dynamic_cast<app::SvgButton *>(child) || dynamic_cast<app::SvgPanel *>(child))
			continue;

		if (auto *light = dynamic_cast<app::ModuleLightWidget *>(child); light && light->getNumColors())
			continue;

		if (!big_enough(child) || isHighlight(child))
			continue;

		class_names.push_back(displayNameFromClass(demangledClassName(child)));
		choices.push_back(
			{"", {.moduleId = moduleId, .x_mm = to_mm(child->box.pos.x), .y_mm = to_mm(child->box.pos.y)}, child});
	}

	auto names = nameDisplays(class_names);
	for (auto i = 0u; i < choices.size(); i++)
		choices[i].name = names[i];

	return choices;
}

// Outline drawn over a display while its menu item is hovered
struct ScreensaverDisplayHighlight : rack::widget::TransparentWidget {
	void draw(const DrawArgs &args) override {
		drawOutline(args);
	}

	// Also draw on the light layer, so it shows over self-lit displays and in a dim room
	void drawLayer(const DrawArgs &args, int layer) override {
		if (layer == 1)
			drawOutline(args);
	}

	void drawOutline(const DrawArgs &args) {
		nvgBeginPath(args.vg);
		nvgRect(args.vg, -2, -2, box.size.x + 4, box.size.y + 4);
		nvgFillColor(args.vg, nvgRGBA(0xfd, 0x8b, 0x18, 0x40));
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(0xfd, 0x8b, 0x18, 0xff));
		nvgStrokeWidth(args.vg, 2);
		nvgStroke(args.vg);
	}
};

// Our own outline is not a display
inline bool isHighlight(rack::widget::Widget *w) {
	return dynamic_cast<ScreensaverDisplayHighlight *>(w) != nullptr;
}

// Menu item for one display: hovering it outlines the display in the rack
struct ScreensaverDisplayMenuItem : rack::ui::MenuItem {
	std::function<bool()> isChosen;
	std::function<void()> choose;
	int64_t moduleId = -1;
	rack::widget::Widget *target = nullptr;
	ScreensaverDisplayHighlight *highlight = nullptr;

	~ScreensaverDisplayMenuItem() override {
		hideHighlight();
	}

	void step() override {
		rightText = CHECKMARK(isChosen && isChosen());
		MenuItem::step();
	}

	void onAction(const ActionEvent &e) override {
		if (choose)
			choose();
	}

	void onEnter(const EnterEvent &e) override {
		MenuItem::onEnter(e);
		showHighlight();
	}

	void onLeave(const LeaveEvent &e) override {
		MenuItem::onLeave(e);
		hideHighlight();
	}

private:
	// The module may have been deleted since the menu was made
	rack::app::ModuleWidget *moduleWidget() {
		return APP->scene->rack->getModule(moduleId);
	}

	void showHighlight() {
		auto *mw = moduleWidget();
		if (highlight || !mw || !target || target->parent != mw)
			return;

		highlight = new ScreensaverDisplayHighlight;
		highlight->box = target->box;
		mw->addChild(highlight);
		scrollIntoView(mw);
	}

	void hideHighlight() {
		if (!highlight)
			return;
		if (auto *mw = moduleWidget()) {
			mw->removeChild(highlight);
			delete highlight;
		}
		highlight = nullptr;
	}

	// Scroll the rack if the module is (partly) out of view
	static void scrollIntoView(rack::app::ModuleWidget *mw) {
		using namespace rack;
		auto *scroll = APP->scene->rackScroll;

		math::Rect module_rect = math::Rect::fromMinMax(mw->getRelativeOffset(math::Vec{}, APP->scene),
														mw->getRelativeOffset(mw->box.size, APP->scene));
		math::Rect view{scroll->getRelativeOffset(math::Vec{}, APP->scene), scroll->box.size};
		if (view.contains(module_rect))
			return;

		// scrollTo() takes a rect in the scroll container's coordinates
		auto container_pos = scroll->container->getRelativeOffset(math::Vec{}, APP->scene);
		scroll->scrollTo(math::Rect{module_rect.pos.minus(container_pos), module_rect.size});
	}
};

} // namespace MetaModule
