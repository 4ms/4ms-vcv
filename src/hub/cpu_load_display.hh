#pragma once
#include <rack.hpp>
#include <string>

namespace MetaModule
{

// MetaModule CPU load under the Hub's Wi-Fi icon, styled like the panel labels:
// a small "CPU" caption with the value below it
struct CpuLoadDisplay : rack::widget::Widget {
	std::string value = "—";

	void draw(const DrawArgs &args) override {
		auto font = APP->window->loadFont(rack::asset::system("res/fonts/Nunito-Bold.ttf"));
		if (!font)
			return;

		nvgFontFaceId(args.vg, font->handle);
		nvgFillColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);

		nvgFontSize(args.vg, 7.f);
		nvgText(args.vg, box.size.x / 2.f, 0.f, "CPU", nullptr);

		nvgFontSize(args.vg, 11.f);
		nvgText(args.vg, box.size.x / 2.f, 8.f, value.c_str(), nullptr);
	}
};

} // namespace MetaModule
