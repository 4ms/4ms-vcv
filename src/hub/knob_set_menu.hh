#pragma once
#include "hub/hub_module.hh"
#include <rack.hpp>

namespace MetaModule
{

struct KnobSetNameTextBox : rack::ui::TextField {
	using CallbackT = std::function<void(unsigned, std::string const &)>;
	CallbackT onChangeCallback;
	unsigned idx;
	unsigned max_chars;

	KnobSetNameTextBox(CallbackT &&callback, unsigned knobSetIdx, unsigned max_chars)
		: onChangeCallback{callback}
		, idx{knobSetIdx}
		, max_chars{max_chars} {
	}

	void onChange(const rack::event::Change &e) override {
		if (text.size() >= max_chars)
			text = text.substr(0, max_chars);

		onChangeCallback(idx, text);

		if (cursor > (int)text.size())
			cursor = text.size();
		if (selection > (int)text.size())
			selection = text.size();
	}
};

struct KnobSetNameMenuItem : rack::widget::Widget {
	KnobSetNameTextBox *txt;

	KnobSetNameMenuItem(KnobSetNameTextBox::CallbackT &&onChangeCallback,
						unsigned knobSetIdx,
						std::string_view initialText,
						unsigned max_chars) {
		box.pos = {0, 0};
		box.size = {250, BND_WIDGET_HEIGHT};
		txt = new KnobSetNameTextBox{std::move(onChangeCallback), knobSetIdx, max_chars};
		txt->box.pos = {45, 0};
		txt->box.size = {230 - txt->box.pos.x, BND_WIDGET_HEIGHT};
		txt->text = initialText;
		addChild(txt);
	}

	void draw(const DrawArgs &args) override {
		bndMenuLabel(args.vg, 0.0, 0.0, box.size.x, box.size.y, -1, "Name:");
		Widget::draw(args);
	}
};

} // namespace MetaModule
