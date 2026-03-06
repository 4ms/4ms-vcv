#pragma once
#include "hub/hub_module.hh"
#include "mapping/map_palette.hh"
#include <rack.hpp>

namespace MetaModule
{

using namespace rack;

struct ModuleAliasLabelWidget : rack::widget::TransparentWidget {
	int64_t moduleId;
	std::string text;
	int colorIdx = 0;

	static constexpr float kWidth = 80.f;
	static constexpr float kHeight = 16.f;

	ModuleAliasLabelWidget(int64_t moduleId, std::string const &text, int colorIdx = 0)
		: moduleId{moduleId}
		, text{text}
		, colorIdx{colorIdx} {
		box.size = Vec(kWidth, kHeight);
	}

	void step() override {
		auto *mw = APP->scene->rack->getModule(moduleId);
		if (mw)
			box.pos = mw->box.pos.plus(Vec((mw->box.size.x - kWidth) / 2.f, 1.f));
		TransparentWidget::step();
	}

	void draw(const DrawArgs &args) override {
		Rect d = Rect(Vec(0.f, 0.f), Vec(kWidth, kHeight));

		// Shadow (matches GLUE LabelDrawWidget)
		nvgBeginPath(args.vg);
		float r = 4.f, c = 4.f;
		Vec b = Vec(-2.f, -2.f);
		nvgRect(args.vg, d.pos.x + b.x - r, d.pos.y + b.y - r,
			d.size.x - 2 * b.x + 2 * r, d.size.y - 2 * b.y + 2 * r);
		nvgFillPaint(args.vg, nvgBoxGradient(args.vg,
			d.pos.x + b.x, d.pos.y + b.y, d.size.x - 2 * b.x, d.size.y - 2 * b.y,
			c, r, nvgRGBAf(0.f, 0.f, 0.f, 0.1f), nvgRGBAf(0.f, 0.f, 0.f, 0.f)));
		nvgFill(args.vg);

		// Background
		nvgBeginPath(args.vg);
		nvgRect(args.vg, d.pos.x, d.pos.y, d.size.x, d.size.y);
		nvgFillColor(args.vg, PaletteHub::color(colorIdx));
		nvgFill(args.vg);

		// Text
		if (!text.empty()) {
			auto font = APP->window->loadFont(asset::system("res/fonts/DejaVuSans.ttf"));
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
	}
};

struct ModuleAliasContainer : rack::widget::Widget {
	MetaModuleHubBase *hubModule;

	ModuleAliasContainer(MetaModuleHubBase *hubModule)
		: hubModule{hubModule} {}

	void step() override {
		Widget::step();
		if (!hubModule)
			return;

		auto &aliases = hubModule->module_aliases;

		// Remove children for cleared aliases or gone modules
		std::vector<Widget *> toRemove;
		for (auto *child : children) {
			auto *lw = dynamic_cast<ModuleAliasLabelWidget *>(child);
			if (!lw)
				continue;
			bool inAliases = aliases.count(lw->moduleId) > 0;
			bool moduleExists = APP->scene->rack->getModule(lw->moduleId) != nullptr;
			if (!inAliases || !moduleExists)
				toRemove.push_back(child);
		}
		for (auto *w : toRemove) {
			removeChild(w);
			delete w;
		}

		// Add/update children for each alias
		for (auto &[id, alias] : aliases) {
			int colorIdx = 0;
			if (auto it = hubModule->module_alias_colors.find(id); it != hubModule->module_alias_colors.end())
				colorIdx = it->second;

			bool found = false;
			for (auto *child : children) {
				auto *lw = dynamic_cast<ModuleAliasLabelWidget *>(child);
				if (lw && lw->moduleId == id) {
					lw->text = alias;
					lw->colorIdx = colorIdx;
					found = true;
					break;
				}
			}
			if (!found)
				addChild(new ModuleAliasLabelWidget{id, alias, colorIdx});
		}
	}
};

struct ModuleAliasTextBox : rack::ui::TextField {
	using CallbackT = std::function<void(int64_t, std::string const &)>;
	CallbackT onChangeCallback;
	int64_t moduleId;
	static constexpr unsigned kMaxChars = 32;

	ModuleAliasTextBox(CallbackT &&callback, int64_t moduleId)
		: onChangeCallback{callback}
		, moduleId{moduleId} {}

	void onChange(const rack::event::Change &e) override {
		if (text.size() >= kMaxChars)
			text = text.substr(0, kMaxChars);
		onChangeCallback(moduleId, text);
		if (cursor > (int)text.size())
			cursor = text.size();
		if (selection > (int)text.size())
			selection = text.size();
	}
};

struct ModuleAliasMenuItem : rack::widget::Widget {
	ModuleAliasTextBox *txt;

	ModuleAliasMenuItem(ModuleAliasTextBox::CallbackT &&onChangeCallback,
						int64_t moduleId,
						std::string const &initialText) {
		box.pos = {0, 0};
		box.size = {250, BND_WIDGET_HEIGHT};
		txt = new ModuleAliasTextBox{std::move(onChangeCallback), moduleId};
		txt->box.pos = {45, 0};
		txt->box.size = {205, BND_WIDGET_HEIGHT};
		txt->text = initialText;
		addChild(txt);
	}

	void draw(const DrawArgs &args) override {
		bndMenuLabel(args.vg, 0.f, 0.f, box.size.x, box.size.y, -1, "Name:");
		Widget::draw(args);
	}
};

} // namespace MetaModule
