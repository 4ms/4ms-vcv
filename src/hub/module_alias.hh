#pragma once
#include "hub/hub_module.hh"
#include "mapping/map_palette.hh"
#include <rack.hpp>

namespace MetaModule
{

using namespace rack;

struct ModuleAliasContainer;

struct ModuleAliasLabelWidget : rack::widget::OpaqueWidget {
	int64_t moduleId;
	std::string text;
	int colorIdx = 0;
	ModuleAliasContainer *container = nullptr;

	static constexpr float kWidth = 80.f;
	static constexpr float kHeight = 16.f;

	bool flashing = false;
	unsigned flashPhase = 0;
	constexpr static unsigned flashRate = 6;

	ModuleAliasLabelWidget(int64_t moduleId,
						   std::string const &text,
						   int colorIdx = 0,
						   ModuleAliasContainer *container = nullptr)
		: moduleId{moduleId}
		, text{text}
		, colorIdx{colorIdx}
		, container{container} {
		box.size = Vec(kWidth, kHeight);
	}

	void createContextMenu();

	void onButton(const ButtonEvent &e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
			createContextMenu();
			e.consume(this);
			return;
		}
		OpaqueWidget::onButton(e);
	}

	void step() override {
		auto *mw = APP->scene->rack->getModule(moduleId);
		if (mw)
			box.pos = mw->box.pos.plus(Vec((mw->box.size.x - kWidth) / 2.f, 1.f));
		flashPhase = flashPhase ? flashPhase - 1 : flashRate;
		OpaqueWidget::step();
	}

	void draw(const DrawArgs &args) override {
		nvgSave(args.vg);
		Rect d = Rect(Vec(0.f, 0.f), Vec(kWidth, kHeight));

		// Shadow (matches GLUE LabelDrawWidget)
		nvgBeginPath(args.vg);
		float r = 4.f, c = 4.f;
		Vec b = Vec(-2.f, -2.f);
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
	int64_t previewColorModuleId = -1;
	int previewColorIdx = -1;

	ModuleAliasContainer(MetaModuleHubBase *hubModule)
		: hubModule{hubModule} {
	}

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
			if (previewColorModuleId == id && previewColorIdx >= 0)
				colorIdx = previewColorIdx;

			auto found = std::find_if(children.begin(), children.end(), [&](auto *child) {
				auto *lw = dynamic_cast<ModuleAliasLabelWidget *>(child);
				return (lw && lw != tempFlashLabel && lw->moduleId == id);
			});
			if (found != children.end()) {
				auto *lw = dynamic_cast<ModuleAliasLabelWidget *>(*found);
				lw->text = alias;
				lw->colorIdx = colorIdx;
			} else {
				addChild(new ModuleAliasLabelWidget{id, alias, colorIdx, this});
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
					tempFlashLabel = new ModuleAliasLabelWidget{flashModuleId, "", colorIdx, this};
					tempFlashLabel->flashing = true;
					addChild(tempFlashLabel);
				}
				if (previewColorModuleId == flashModuleId && previewColorIdx >= 0)
					tempFlashLabel->colorIdx = previewColorIdx;
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
		, container{container} {
	}

	~ModuleAliasTextBox() {
		if (container) {
			if (container->flashModuleId == moduleId)
				container->flashModuleId = -1;
			// Clean up empty aliases when menu closes
			if (container->hubModule) {
				auto &aliases = container->hubModule->module_aliases;
				if (auto it = aliases.find(moduleId); it != aliases.end() && it->second.empty()) {
					aliases.erase(it);
					container->hubModule->module_alias_colors.erase(moduleId);
				}
			}
		}
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

struct ModuleAliasColorMenuItem : rack::widget::Widget {
	int64_t moduleId;
	MetaModuleHubBase *hubModule;
	ModuleAliasContainer *container;
	int hoveredIdx = -1;

	static constexpr float kSwatchSize = 14.f;
	static constexpr float kSwatchSpacing = 2.f;
	static constexpr float kLabelWidth = 45.f;
	static constexpr unsigned kNumColors = 12;

	ModuleAliasColorMenuItem(int64_t moduleId, MetaModuleHubBase *hubModule, ModuleAliasContainer *container)
		: moduleId{moduleId}
		, hubModule{hubModule}
		, container{container} {
		box.size = {kLabelWidth + kNumColors * (kSwatchSize + kSwatchSpacing), BND_WIDGET_HEIGHT};
	}

	~ModuleAliasColorMenuItem() {
		clearPreview();
	}

	int swatchAtPos(Vec pos) {
		float x = pos.x - kLabelWidth;
		if (x < 0)
			return -1;
		int idx = (int)(x / (kSwatchSize + kSwatchSpacing));
		float localX = x - idx * (kSwatchSize + kSwatchSpacing);
		if (idx >= 0 && idx < (int)kNumColors && localX <= kSwatchSize)
			return idx;
		return -1;
	}

	void setPreviewColor(int colorIdx) {
		if (!container)
			return;
		container->previewColorModuleId = moduleId;
		container->previewColorIdx = colorIdx;
	}

	void clearPreview() {
		if (container && container->previewColorModuleId == moduleId) {
			container->previewColorModuleId = -1;
			container->previewColorIdx = -1;
		}
	}

	int currentColorIdx() {
		if (auto it = hubModule->module_alias_colors.find(moduleId); it != hubModule->module_alias_colors.end())
			return it->second;
		return -1;
	}

	void onHover(const HoverEvent &e) override {
		e.consume(this);
		int idx = swatchAtPos(e.pos);
		if (idx != hoveredIdx) {
			if (idx >= 0) {
				setPreviewColor(idx);
			} else {
				clearPreview();
			}
			hoveredIdx = idx;
		}
	}

	void onEnter(const EnterEvent &e) override {
		if (container)
			container->flashModuleId = moduleId;
	}

	void onLeave(const LeaveEvent &e) override {
		clearPreview();
		hoveredIdx = -1;
		if (container && container->flashModuleId == moduleId)
			container->flashModuleId = -1;
	}

	void onButton(const ButtonEvent &e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			int idx = swatchAtPos(e.pos);
			if (idx >= 0) {
				hubModule->module_alias_colors[moduleId] = idx;
				e.consume(this);
			}
		}
	}

	void draw(const DrawArgs &args) override {
		bndMenuLabel(args.vg, 0.f, 0.f, box.size.x, box.size.y, -1, "Color:");

		int selected = currentColorIdx();
		float y = (box.size.y - kSwatchSize) / 2.f;

		for (unsigned i = 0; i < kNumColors; i++) {
			float x = kLabelWidth + i * (kSwatchSize + kSwatchSpacing);

			nvgBeginPath(args.vg);
			nvgRect(args.vg, x, y, kSwatchSize, kSwatchSize);
			nvgFillColor(args.vg, PaletteHub::color(i));
			nvgFill(args.vg);

			// Highlight the currently selected color
			if (selected >= 0 && (int)i == selected % (int)kNumColors) {
				nvgStrokeWidth(args.vg, 2.f);
				nvgStrokeColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
				nvgStroke(args.vg);
			}

			// Hover outline
			if ((int)i == hoveredIdx) {
				nvgStrokeWidth(args.vg, 1.5f);
				nvgStrokeColor(args.vg, nvgRGB(0xcc, 0xcc, 0xcc));
				nvgStroke(args.vg);
			}
		}

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

inline void ModuleAliasLabelWidget::createContextMenu() {
	if (!container || !container->hubModule)
		return;
	auto *hubModule = container->hubModule;

	auto *menu = rack::createMenu();

	std::string currentAlias;
	if (auto it = hubModule->module_aliases.find(moduleId); it != hubModule->module_aliases.end())
		currentAlias = it->second;

	menu->addChild(new ModuleAliasMenuItem{[hubModule](int64_t id, std::string const &text) {
											   if (!hubModule->module_alias_colors.count(id))
												   hubModule->module_alias_colors[id] =
													   hubModule->module_alias_colors.size();
											   hubModule->module_aliases[id] = text;
										   },
										   moduleId,
										   currentAlias,
										   container});

	menu->addChild(new ModuleAliasColorMenuItem{moduleId, hubModule, container});
}

} // namespace MetaModule
