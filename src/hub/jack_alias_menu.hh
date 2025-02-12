#pragma once

#include "hub/knob_set_menu.hh"
#include <rack.hpp>

namespace MetaModule
{

struct JackNameMenuItem : rack::widget::Widget {
	KnobSetNameTextBox *txt;

	JackNameMenuItem(KnobSetNameTextBox::CallbackT &&onChangeCallback,
					 unsigned knobSetIdx,
					 std::string_view placeholder,
					 std::string_view initialText,
					 unsigned max_chars) {
		box.pos = {0, 0};
		box.size = {250, BND_WIDGET_HEIGHT};
		txt = new KnobSetNameTextBox{std::move(onChangeCallback), knobSetIdx, max_chars};
		txt->box.pos = {10, 0};
		txt->box.size = {230, BND_WIDGET_HEIGHT};
		txt->placeholder = placeholder;
		txt->text = initialText;
		addChild(txt);
	}
};

} // namespace MetaModule
