#pragma once
#include <rack.hpp>
#include <string_view>

struct MetaModuleTextField : rack::LedDisplayTextField {
	using CallbackT = std::function<void(std::string &)>;
	CallbackT onChangeCallback;
	unsigned max_chars;

	MetaModuleTextField(CallbackT &&callback, unsigned max_chars)
		: onChangeCallback{callback}
		, max_chars{max_chars} {
	}

	void onChange(const ChangeEvent &event) override {
		if (text.size() >= max_chars)
			text = text.substr(0, max_chars);

		onChangeCallback(text);

		if (cursor > (int)text.size())
			cursor = text.size();
		if (selection > (int)text.size())
			selection = text.size();
	}
};
