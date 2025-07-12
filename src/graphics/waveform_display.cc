#include "graphics/waveform_display.hh"
#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/elements/units.hh"
// #include "thorvg/thorvg/inc/thorvg.h"
#include <algorithm>
#include <atomic>
#include <cmath>

namespace MetaModule
{

struct StreamingWaveformDisplay::Internal {
	// tvg::SwCanvas *canvas = nullptr;
	// tvg::Shape *bar_cursor = nullptr;
	// tvg::Shape *wave = nullptr;
};

// TSP: (24mm, 23.4mm)
StreamingWaveformDisplay::StreamingWaveformDisplay(float display_width_mm, float display_height_mm)
	: internal{new Internal}
	, display_width{mm_to_px(display_width_mm, 240)}
	, display_height{mm_to_px(display_height_mm, 240)}
	, wave_height{(display_height - bar_height) / 2.f} {

	samples.resize(320);
}

StreamingWaveformDisplay::~StreamingWaveformDisplay() = default;

void StreamingWaveformDisplay::set_cursor_position(float pos) {
	cursor_pos = pos;
}

void StreamingWaveformDisplay::draw_sample(float sample) {
	sample = std::clamp(sample, -1.f, 1.f);

	oversample_min = std::min(oversample_min, sample);
	oversample_max = std::max(oversample_max, sample);

	x_zoom_ctr += x_zoom;
	if (x_zoom_ctr > 1.f) {
		x_zoom_ctr = 0;

		{
			auto t = newest_sample.load();

			if (++t >= (int)samples.size())
				t = 0;

			samples[t] = {oversample_min, oversample_max};

			newest_sample.store(t);
		}

		oversample_min = 1.f;
		oversample_max = -1.f;
	}
}

void StreamingWaveformDisplay::sync() {
	std::ranges::fill(samples, std::pair<float, float>{0, 0});
	newest_sample = 0;
}

void StreamingWaveformDisplay::set_x_zoom(float zoom) {
	if (zoom > 0)
		x_zoom = 1.f / zoom;
	else
		x_zoom = 1;
}

void StreamingWaveformDisplay::set_wave_color(uint8_t r, uint8_t g, uint8_t b) {
	wave_r = r;
	wave_g = g;
	wave_b = b;
}

void StreamingWaveformDisplay::set_bar_color(uint8_t r, uint8_t g, uint8_t b) {
	bar_r = r;
	bar_g = g;
	bar_b = b;
}

void StreamingWaveformDisplay::set_cursor_width(unsigned width) {
	cursor_width = width;
}

// Functions below here run in the GUI thread and may get interrupted by the functions above

void StreamingWaveformDisplay::show_graphic_display(std::span<uint32_t> pix_buffer,
													unsigned width,
													lv_obj_t *lvgl_canvas) {
	// internal->canvas = tvg::SwCanvas::gen();
	// auto scene = tvg::Scene::gen();

	// auto scaled_height = pix_buffer.size() / width;
	// internal->canvas->target(pix_buffer.data(), width, width, scaled_height, tvg::ColorSpace::ARGB8888);
	// scaling = float(width) / display_width;
	// scene->scale(scaling);

	// // Bar to represent entire sample
	// auto bar = tvg::Shape::gen();
	// bar->appendRect(0, display_height - bar_height / scaling, display_width, bar_height / scaling);
	// bar->fill(bar_r, bar_g, bar_b, 0xFF);
	// scene->push(bar);

	// // Trolley to indicate position
	// internal->bar_cursor = tvg::Shape::gen();
	// internal->bar_cursor->appendRect(0, display_height - bar_height / scaling, cursor_width, bar_height / scaling);
	// internal->bar_cursor->fill(0xFF, 0xFF, 0xFF, 0xFF);
	// scene->push(internal->bar_cursor);

	// // Black background
	// auto wave_bg = tvg::Shape::gen();
	// wave_bg->appendRect(0, 0, display_width, display_height - bar_height);
	// wave_bg->fill(0x00, 0x00, 0x00, 0xFF);
	// scene->push(wave_bg);

	// // Waveform
	// internal->wave = tvg::Shape::gen();
	// internal->wave->moveTo(display_width, wave_height);
	// internal->wave->lineTo(0, wave_height);
	// internal->wave->strokeFill(wave_r, wave_g, wave_b, 0xFF);
	// internal->wave->strokeWidth(1.0f);
	// scene->push(internal->wave);

	// internal->canvas->push(scene);
}

bool StreamingWaveformDisplay::draw_graphic_display() {
	//if (!internal->canvas)
	//	return false;

	//internal->bar_cursor->translate(scaling * cursor_pos * (display_width - cursor_width), 0);
	//internal->bar_cursor->scale(scaling);
	//internal->canvas->update(internal->bar_cursor);

	//internal->wave->reset();

	////start with the oldest sample
	//int start = newest_sample.load() + 1;

	//// Draw top contour left->right
	//int i = start;
	//for (auto x = 0u; x < samples.size(); x++) {
	//	float x_pos = (float)x * display_width / (float)samples.size();
	//	float y_pos = samples[i].first * wave_height + wave_height;
	//	if (x == 0)
	//		internal->wave->moveTo(x_pos, y_pos);
	//	else
	//		internal->wave->lineTo(x_pos, y_pos);
	//	i = (i + 1) % samples.size();
	//}

	//// Continue shape, drawing bottom contour right->left
	//for (int x = samples.size() - 1; x >= 0; x--) {
	//	if (--i < 0)
	//		i = samples.size() - 1;
	//	float x_pos = (float)x * display_width / (float)samples.size();
	//	float y_pos = samples[i].second * wave_height + wave_height;
	//	internal->wave->lineTo(x_pos, y_pos);
	//}

	//internal->wave->strokeWidth(1.0f / scaling);
	//internal->wave->strokeFill(wave_r, wave_g, wave_b, 0xFF);
	//internal->wave->fill(wave_r, wave_g, wave_b, 0x7F);
	//internal->wave->scale(scaling);

	//internal->canvas->update(internal->wave);
	//internal->canvas->draw();
	//internal->canvas->sync();

	return true;
}

void StreamingWaveformDisplay::hide_graphic_display() {
	// if (internal->canvas) {
	// 	delete internal->canvas;
	// 	internal->canvas = nullptr;
	// }
}

} // namespace MetaModule
