#pragma once

#include <rack.hpp>

namespace MetaModule
{
struct HubSaveButton : rack::BefacoPush {
	std::function<void(void)> click_callback;

	void onDragEnd(const rack::event::DragEnd &e) override {
		OpaqueWidget::onDragEnd(e);
		if (click_callback) {
			click_callback();
		}
	}
};

struct HubWifiButton : rack::VCVBezel {
	std::function<void(void)> click_callback;

	rack::LedDisplayTextField *label;
	std::string const &wifiUrlText;
	std::string cachedText;

	HubWifiButton(rack::LedDisplayTextField *label, std::string const &wifiUrlText)
		: label{label}
		, wifiUrlText{wifiUrlText} {
	}

	void onDragEnd(const rack::event::DragEnd &e) override {
		OpaqueWidget::onDragEnd(e);
		if (click_callback) {
			click_callback();
		}
	}

	void onHover(rack::event::Hover const &e) override {
		e.consume(this);
	}

	void onEnter(rack::event::Enter const &) override {
		if (wifiUrlText.length()) {
			cachedText = label->text;
			label->text = wifiUrlText;
		}
	}

	void onLeave(rack::event::Leave const &) override {
		if (wifiUrlText.length()) {
			label->text = cachedText;
		}
	}
};

} // namespace MetaModule
