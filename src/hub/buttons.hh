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

	rack::Label *wifiURLText;

	HubWifiButton(rack::Label *label)
		: wifiURLText{label} {
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
		wifiURLText->show();
	}

	void onLeave(rack::event::Leave const &) override {
		wifiURLText->hide();
	}
};

} // namespace MetaModule
