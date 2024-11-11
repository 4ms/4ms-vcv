#pragma once
#include "mm_blendish.hh"
#include <rack.hpp>
#include <string_view>

namespace MetaModule
{

class TextField : public rack::LedDisplayTextField {
	using CallbackT = std::function<void(std::string &)>;
	CallbackT onChangeCallback;
	unsigned max_chars;

public:
	TextField(CallbackT &&callback, unsigned max_chars)
		: onChangeCallback{callback}
		, max_chars{max_chars} {
	}

	TextField()
		: onChangeCallback{nullptr}
		, max_chars{0} {
	}

	void onChange(const ChangeEvent &event) override {
		if (max_chars) {
			if (text.size() >= max_chars)
				text = text.substr(0, max_chars);

			if (cursor > (int)text.size())
				cursor = text.size();

			if (selection > (int)text.size())
				selection = text.size();
		}

		if (onChangeCallback) {
			onChangeCallback(text);
		}
	}

private:
	void drawLayer(const DrawArgs &args, int layer) override {
		nvgScissor(args.vg, RECT_ARGS(args.clipBox));

		if (layer == 1) {
			// Text
			std::shared_ptr<rack::window::Font> font = APP->window->loadFont(fontPath);
			if (font && font->handle >= 0) {
				MetaModule::bndSetFont(font->handle);

				NVGcolor highlightColor = color;
				highlightColor.a = 0.5;
				int begin = std::min(cursor, selection);
				int end = (this == APP->event->selectedWidget) ? std::max(cursor, selection) : -1;
				MetaModule::bndIconLabelCaret(args.vg,
											  textOffset.x,
											  textOffset.y,
											  box.size.x - 2 * textOffset.x,
											  box.size.y - 2 * textOffset.y,
											  -1,
											  color,
											  12,
											  text.c_str(),
											  highlightColor,
											  begin,
											  end);

				MetaModule::bndSetFont(APP->window->uiFont->handle);
			}
		}

		Widget::drawLayer(args, layer);
		nvgResetScissor(args.vg);
	}
};

class LabelOverlay : public MetaModule::TextField {

public:
	void onDragHover(const DragHoverEvent &) override {
	}
	void onButton(const ButtonEvent &) override {
	}
	void onSelectText(const SelectTextEvent &) override {
	}
	void onSelectKey(const SelectKeyEvent &) override {
	}
};

// Clears text after a delay
class LabelDelay : public LabelOverlay {
	rack::Widget *bgWidget;
	unsigned timeToHide = 0;

public:
	LabelDelay(rack::Widget *bgWidget)
		: bgWidget{bgWidget} {
	}

	void showFor(unsigned tm) {
		LabelOverlay::show();
		timeToHide = tm;
	}

	void step() override {
		if (this->isVisible()) {
			if (timeToHide) {
				if (--timeToHide == 0) {
					this->hide();
					bgWidget->show();
				} else {
					bgWidget->hide();
				}
			}
			LabelOverlay::step();
		}
	}

	void onButton(const ButtonEvent &ev) override {
		this->hide();
		bgWidget->show();
	}
};

} // namespace MetaModule
