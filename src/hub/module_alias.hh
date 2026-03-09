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

	bool flashing = false;
	unsigned flashPhase = 0;
	constexpr static unsigned flashRate = 6;

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
		flashPhase = flashPhase ? flashPhase - 1 : flashRate;
		TransparentWidget::step();
	}

	void draw(const DrawArgs &args) override {
		nvgSave(args.vg);
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
		if (flashing) {
			float alpha = (flashPhase > flashRate / 2);
			nvgStrokeWidth(args.vg, 2);
			nvgStrokeColor(args.vg, nvgRGBAf(0, 0, 0, alpha));
			nvgStroke(args.vg);
		}

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

		nvgRestore(args.vg);
	}
};

struct ModuleAliasContainer : rack::widget::Widget {
	MetaModuleHubBase *hubModule;
	int64_t flashModuleId = -1;
	ModuleAliasLabelWidget *tempFlashLabel = nullptr;

	ModuleAliasContainer(MetaModuleHubBase *hubModule)
		: hubModule{hubModule} {}

	void removeTempLabelIfPresent() {
		// Remove temporary flash label if present
		if (tempFlashLabel) {
			removeChild(tempFlashLabel);
			delete tempFlashLabel;
			tempFlashLabel = nullptr;
		}
	}

	void step() override {
		Widget::step();
		if (!hubModule)
			return;

		auto &aliases = hubModule->module_aliases;

		// Remove children for cleared aliases or gone modules
		std::vector<Widget *> toRemove;
		for (auto *child : children) {
			auto *lw = dynamic_cast<ModuleAliasLabelWidget *>(child);
			if (!lw || lw == tempFlashLabel)
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

			auto found = std::find_if(children.begin(), children.end(), [&](auto *child) {
				auto *lw = dynamic_cast<ModuleAliasLabelWidget *>(child);
				return (lw && lw != tempFlashLabel && lw->moduleId == id);
			});
			if (found != children.end()) {
				auto *lw = dynamic_cast<ModuleAliasLabelWidget *>(*found);
				lw->text = alias;
				lw->colorIdx = colorIdx;
			} else {
				addChild(new ModuleAliasLabelWidget{id, alias, colorIdx});
			}

		}

		// Reset flashing on all permanent labels
		for (auto *child : children) {
			auto *lw = dynamic_cast<ModuleAliasLabelWidget *>(child);
			if (lw && lw != tempFlashLabel)
				lw->flashing = false;
		}


		// Handle flash highlight
		if (flashModuleId >= 0) {

			// Check if a permanent label exists for the flashed module
			auto found = std::find_if(children.begin(), children.end(), [&](auto *child) {
				auto *lw = dynamic_cast<ModuleAliasLabelWidget *>(child);
				return (lw && lw != tempFlashLabel && lw->moduleId == flashModuleId);
			});

			if (found != children.end()) {
				if (auto *permanentLabel = dynamic_cast<ModuleAliasLabelWidget *>(*found)) {
					// Flash the existing label
					permanentLabel->flashing = true;
					removeTempLabelIfPresent();
				}
			} else {
				// Create or update temporary flash label
				if (!tempFlashLabel || tempFlashLabel->moduleId != flashModuleId) {
					removeTempLabelIfPresent();

					// Use next color
					int colorIdx = hubModule->module_alias_colors.size();
					tempFlashLabel = new ModuleAliasLabelWidget{flashModuleId, "", colorIdx};
					tempFlashLabel->flashing = true;
					addChild(tempFlashLabel);
				}
				tempFlashLabel->flashing = true;
			}
		} else {
			// No flash active — remove temporary label
			removeTempLabelIfPresent();
		}

	}
};

struct ModuleAliasTextBox : rack::ui::TextField {
	using CallbackT = std::function<void(int64_t, std::string const &)>;
	CallbackT onChangeCallback;
	int64_t moduleId;
	ModuleAliasContainer *container = nullptr;
	static constexpr unsigned kMaxChars = 32;

	ModuleAliasTextBox(CallbackT &&callback, int64_t moduleId, ModuleAliasContainer *container)
		: onChangeCallback{callback}
		, moduleId{moduleId}
		, container{container} {}

	~ModuleAliasTextBox() {
		if (container && container->flashModuleId == moduleId)
			container->flashModuleId = -1;
	}

	void onEnter(const EnterEvent &e) override {
		if (container)
			container->flashModuleId = moduleId;
		TextField::onEnter(e);
	}

	void onLeave(const LeaveEvent &e) override {
		if (container && container->flashModuleId == moduleId)
			container->flashModuleId = -1;
		TextField::onLeave(e);
	}

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
						std::string const &initialText,
						ModuleAliasContainer *container = nullptr) {
		box.pos = {0, 0};
		box.size = {250, BND_WIDGET_HEIGHT};
		txt = new ModuleAliasTextBox{std::move(onChangeCallback), moduleId, container};
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

struct ModuleAliasHeaderLabel : rack::ui::MenuLabel {
	ModuleAliasContainer *container = nullptr;
	int64_t moduleId = -1;

	ModuleAliasHeaderLabel(std::string const &text, int64_t moduleId, ModuleAliasContainer *container)
		: container{container}
		, moduleId{moduleId} {
		this->text = text;
	}

	~ModuleAliasHeaderLabel() {
		if (container && container->flashModuleId == moduleId)
			container->flashModuleId = -1;
	}

	void onHover(const HoverEvent &e) override {
		e.consume(this);
	}

	void onEnter(const EnterEvent &e) override {
		if (container)
			container->flashModuleId = moduleId;
		MenuLabel::onEnter(e);
	}

	void onLeave(const LeaveEvent &e) override {
		if (container && container->flashModuleId == moduleId)
			container->flashModuleId = -1;
		MenuLabel::onLeave(e);
	}
};


} // namespace MetaModule
