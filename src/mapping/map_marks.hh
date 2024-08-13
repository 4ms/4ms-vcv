#pragma once
#include <rack.hpp>

struct MapMark {
	MapMark() = delete;

	static void markInputJack(NVGcontext *vg, rack::math::Rect box, NVGcolor color) {
		const float base_size = 4.f;
		const float extraHeight = 3.f;
		const float widthRatio = 0.80f;
		nvgBeginPath(vg);
		nvgRect(vg,
				box.size.x - base_size * widthRatio,
				box.size.y - base_size - extraHeight,
				base_size * widthRatio,
				base_size + extraHeight);
		nvgFillColor(vg, color);
		nvgFill(vg);
		nvgStrokeColor(vg, rack::color::mult(color, 0.5f));
		nvgStrokeWidth(vg, 1.0f);
		nvgStroke(vg);
	}

	static void markOutputJack(NVGcontext *vg, rack::math::Rect box, NVGcolor color) {
		const float radius = 4.f;
		nvgBeginPath(vg);
		nvgCircle(vg, box.size.x - radius, box.size.y - radius, radius);
		nvgFillColor(vg, color);
		nvgFill(vg);
		nvgStrokeColor(vg, rack::color::mult(color, 0.5f));
		nvgStrokeWidth(vg, 1.0f);
		nvgStroke(vg);
	}

	static void markKnob(NVGcontext *vg, rack::math::Rect box, NVGcolor color) {
		const float radius = 6.f;
		nvgBeginPath(vg);
		nvgRect(vg, box.size.x - radius, box.size.y - radius, radius, radius);
		nvgFillColor(vg, color);
		nvgFill(vg);
		nvgStrokeColor(vg, rack::color::mult(color, 0.5));
		nvgStrokeWidth(vg, 1.0);
		nvgStroke(vg);
	}
};
