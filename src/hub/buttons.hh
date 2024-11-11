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

// When hovered, shows the fg widget and hides the bg widget
// When not hovered, shows the bg and hides the fg
struct HubWifiButton : rack::VCVBezel {
	std::function<void(void)> click_callback;

	rack::Widget *fgLabel;
	rack::Widget *bgLabel;

	HubWifiButton(rack::Widget *fglabel, rack::Widget *bglabel)
		: fgLabel{fglabel}
		, bgLabel{bglabel} {
		fgLabel->hide();
		bgLabel->show();
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
		fgLabel->show();
		bgLabel->hide();
	}

	void onLeave(rack::event::Leave const &) override {
		fgLabel->hide();
		bgLabel->show();
	}
};

} // namespace MetaModule
