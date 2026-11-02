#pragma once
#include "mapping/map_palette.hh"
#include <map>
#include <rack.hpp>
#include <string>

namespace MetaModule
{

// A module's CPU load on the MetaModule, drawn at the bottom of the module
// (where Rack's CPU meter goes). Same look as the module alias labels.
struct ModuleLoadLabel : rack::widget::Widget {
	int64_t moduleId;
	std::string text;
	float maxWidth;
	bool fitToModule; // false: fixed width, even if wider than the module

	static constexpr float kHeight = 16.f;

	ModuleLoadLabel(int64_t moduleId, float maxWidth, bool fitToModule)
		: moduleId{moduleId}
		, maxWidth{maxWidth}
		, fitToModule{fitToModule} {
		box.size = rack::Vec(maxWidth, kHeight);
	}

	void step() override {
		if (auto *mw = APP->scene->rack->getModule(moduleId)) {
			float width = fitToModule ? std::max(std::min(maxWidth, mw->box.size.x - 2.f), 0.f) : maxWidth;
			box.size = rack::Vec(width, kHeight);
			box.pos = mw->box.pos.plus(rack::Vec((mw->box.size.x - width) / 2.f, mw->box.size.y - kHeight - 1.f));
		}
		Widget::step();
	}

	void draw(const DrawArgs &args) override {
		nvgSave(args.vg);
		rack::Rect d = rack::Rect(rack::Vec(0.f, 0.f), box.size);

		// Shadow (matches ModuleAliasLabelWidget and GLUE)
		nvgBeginPath(args.vg);
		float r = 4.f, c = 4.f;
		rack::Vec b = rack::Vec(-2.f, -2.f);
		nvgRect(args.vg, d.pos.x + b.x - r, d.pos.y + b.y - r, d.size.x - 2 * b.x + 2 * r, d.size.y - 2 * b.y + 2 * r);
		nvgFillPaint(args.vg,
					 nvgBoxGradient(args.vg,
									d.pos.x + b.x,
									d.pos.y + b.y,
									d.size.x - 2 * b.x,
									d.size.y - 2 * b.y,
									c,
									r,
									nvgRGBAf(0.f, 0.f, 0.f, 0.1f),
									nvgRGBAf(0.f, 0.f, 0.f, 0.f)));
		nvgFill(args.vg);

		// Background
		nvgBeginPath(args.vg);
		nvgRect(args.vg, d.pos.x, d.pos.y, d.size.x, d.size.y);
		nvgFillColor(args.vg, PaletteHub::GRAY75);
		nvgFill(args.vg);

		// Text
		if (!text.empty()) {
			auto font = APP->window->loadFont(rack::asset::system("res/fonts/DejaVuSans.ttf"));
			if (font) {
				nvgFontSize(args.vg, kHeight);
				nvgFontFaceId(args.vg, font->handle);
				nvgTextLetterSpacing(args.vg, 0.f);
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
				nvgFillColor(args.vg, nvgRGB(0x08, 0x08, 0x08));
				NVGtextRow textRow;
				nvgTextBreakLines(args.vg, text.c_str(), nullptr, d.size.x, &textRow, 1);
				nvgTextBox(args.vg, d.pos.x, d.pos.y + 0.2f, d.size.x, textRow.start, textRow.end);
			}
		}

		nvgRestore(args.vg);
	}
};

// Holds one ModuleLoadLabel per module that has a text to show
struct ModuleLoadContainer : rack::widget::Widget {
	int64_t wideLabelModuleId = -1; // this module's label may be wider (the Hub's)

	static constexpr float kModuleLabelWidth = 90.f;	 // fixed, fits "100.0% C2"
	static constexpr float kWideLabelWidth = 1000.f; // as wide as the module

	// texts: VCV module id -> text. Modules not in the map get no label.
	void setTexts(std::map<int64_t, std::string> const &texts) {
		std::vector<Widget *> toRemove;
		for (auto *child : children) {
			auto *label = static_cast<ModuleLoadLabel *>(child);
			if (!texts.contains(label->moduleId) || !APP->scene->rack->getModule(label->moduleId))
				toRemove.push_back(child);
		}
		for (auto *w : toRemove) {
			removeChild(w);
			delete w;
		}

		for (auto &[id, text] : texts) {
			if (!APP->scene->rack->getModule(id))
				continue;

			auto found = std::find_if(children.begin(), children.end(), [id](auto *child) {
				return static_cast<ModuleLoadLabel *>(child)->moduleId == id;
			});

			if (found != children.end()) {
				static_cast<ModuleLoadLabel *>(*found)->text = text;
			} else {
				bool wide = id == wideLabelModuleId;
				auto *label = new ModuleLoadLabel{id, wide ? kWideLabelWidth : kModuleLabelWidth, wide};
				label->text = text;
				addChild(label);
			}
		}
	}

	void clear() {
		clearChildren();
	}
};

} // namespace MetaModule
