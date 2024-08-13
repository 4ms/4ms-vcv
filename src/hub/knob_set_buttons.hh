#pragma once
#include <rack.hpp>

struct KnobSetButtonGroup;

struct KnobSetButton : rack::Widget {
	int set_idx;
	KnobSetButtonGroup *parent_group;

	KnobSetButton(int index, KnobSetButtonGroup *parent)
		: set_idx{index}
		, parent_group{parent} {
	}

	void drawLayer(const DrawArgs &args, int layer) override;
	void onDragEnter(const rack::event::DragEnter &e) override;
	void onButton(const rack::event::Button &e) override;
	void onDragHover(const rack::event::DragHover &e) override;
};

struct KnobSetButtonGroup : rack::OpaqueWidget {
	std::function<void(unsigned)> onChange;
	unsigned active_idx = 0;

	KnobSetButtonGroup(std::function<void(unsigned)> &&onChangeCallback, rack::Vec pos)
		: onChange{std::move(onChangeCallback)} {
		box.pos = pos;
		const unsigned num_sets = 8;
		const unsigned cols = 8;
		const unsigned rows = num_sets / cols;

		const float spacing = rack::mm2px(6.5f);

		box.size = rack::mm2px(rack::Vec(spacing * cols, spacing * rows));

		rack::math::Rect bbox;
		bbox.size = rack::math::Vec(spacing, spacing);
		bbox.pos = rack::math::Vec(0, 0);

		for (unsigned i = 0; i < num_sets; i++) {
			auto button = new KnobSetButton(i, this);
			button->box = bbox;
			addChild(button);
			if ((i % cols) == (cols - 1)) {
				bbox.pos.x = 0;
				bbox.pos.y += spacing;
			} else
				bbox.pos.x += spacing;
		}
	}
};

inline void KnobSetButton::drawLayer(const DrawArgs &args, int layer) {
	if (layer != 1)
		return;

	const rack::Vec center = box.size.div(2);
	const float radius = rack::mm2px(2);
	int active_idx = parent_group ? parent_group->active_idx : 0;
	auto color = (active_idx == set_idx) ? rack::SCHEME_YELLOW : rack::color::alpha(rack::color::WHITE, 0.33);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, radius);
	nvgFillColor(args.vg, color);
	nvgFill(args.vg);
}

inline void KnobSetButton::onDragEnter(const rack::event::DragEnter &e) {
	if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
		if (parent_group && e.origin == parent_group) {
			parent_group->active_idx = set_idx;
			parent_group->onChange(set_idx);
		}
	}
	Widget::onDragEnter(e);
}

inline void KnobSetButton::onButton(const rack::event::Button &e) {
	if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
		if (parent_group) {
			parent_group->active_idx = set_idx;
			parent_group->onChange(set_idx);
		}
	}
	Widget::onButton(e);
}

inline void KnobSetButton::onDragHover(const rack::event::DragHover &e) {
	if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
		e.consume(this);
	}
	Widget::onDragHover(e);
}
