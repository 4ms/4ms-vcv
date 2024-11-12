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

	rack::Widget *fgLabel = nullptr;
	rack::Widget *bgLabel = nullptr;

	void setLabels(rack::Widget *fglabel, rack::Widget *bglabel) {
		this->fgLabel = fglabel;
		this->bgLabel = bglabel;
		this->fgLabel->hide();
		this->bgLabel->show();
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
		if (fgLabel)
			fgLabel->show();
		if (bgLabel)
			bgLabel->hide();
	}

	void onLeave(rack::event::Leave const &) override {
		if (fgLabel)
			fgLabel->hide();
		if (bgLabel)
			bgLabel->show();
	}
};

} // namespace MetaModule
