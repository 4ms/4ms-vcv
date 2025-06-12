#pragma once
#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/elements/units.hh"
#include <algorithm>
#include <atomic>
#include <vector>

namespace MetaModule
{

class StreamingWaveformDisplay {
public:
	StreamingWaveformDisplay(float display_width_mm, float display_height_mm)
		: display_width{mm_to_px(display_width_mm, 240)}
		, display_height{mm_to_px(display_height_mm, 240)}
		, wave_height{(display_height - bar_height) / 2.f} {

		samples.resize(320);
	}

	void set_cursor_position(float pos) {
		cursor_pos = pos;
	}

	void draw_sample(float sample) {
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

	void sync() {
		std::ranges::fill(samples, std::pair<float, float>{0, 0});
		newest_sample = 0;
	}

	void set_x_zoom(float zoom) {
		x_zoom = zoom;
	}

	void set_wave_color(uint8_t r, uint8_t g, uint8_t b) {
		wave_r = r;
		wave_g = g;
		wave_b = b;
	}
	void set_bar_color(uint8_t r, uint8_t g, uint8_t b) {
		bar_r = r;
		bar_g = g;
		bar_b = b;
	}

	// Functions below here run in the GUI thread and may get interrupted by the functions above

	void show_graphic_display(std::span<uint32_t> pix_buffer, unsigned width, lv_obj_t *lvgl_canvas) {
		// canvas = tvg::SwCanvas::gen();
		// auto scene = tvg::Scene::gen();

		// auto scaled_height = pix_buffer.size() / width;
		// canvas->target(pix_buffer.data(), width, width, scaled_height, tvg::ColorSpace::ARGB8888);
		// scaling = float(width) / display_width;
		// scene->scale(scaling);

		// // Bar to represent entire sample
		// auto bar = tvg::Shape::gen();
		// bar->appendRect(0, display_height - bar_height / scaling, display_width, bar_height / scaling);
		// bar->fill(bar_r, bar_g, bar_b, 0xFF);
		// scene->push(bar);

		// // Trolley to indicate position
		// bar_cursor = tvg::Shape::gen();
		// bar_cursor->appendRect(0, display_height - bar_height / scaling, cursor_width, bar_height / scaling);
		// bar_cursor->fill(0xFF, 0xFF, 0xFF, 0xFF);
		// scene->push(bar_cursor);

		// // Black background
		// auto wave_bg = tvg::Shape::gen();
		// wave_bg->appendRect(0, 0, display_width, display_height - bar_height);
		// wave_bg->fill(0x00, 0x00, 0x00, 0xFF);
		// scene->push(wave_bg);

		// // Waveform
		// wave = tvg::Shape::gen();
		// wave->moveTo(display_width, wave_height);
		// wave->lineTo(0, wave_height);
		// wave->strokeFill(wave_r, wave_g, wave_b, 0xFF);
		// wave->strokeWidth(1.0f);
		// scene->push(wave);

		// canvas->push(scene);
	}

	bool draw_graphic_display() {
		// if (!canvas)
		// 	return false;

		// bar_cursor->translate(scaling * cursor_pos * (display_width - cursor_width), 0);
		// bar_cursor->scale(scaling);
		// canvas->update(bar_cursor);

		// wave->reset();

		//start with the oldest sample
		int start = newest_sample.load() + 1;

		// Draw top contour left->right
		int i = start;
		for (auto x = 0u; x < samples.size(); x++) {
			float x_pos = (float)x * display_width / (float)samples.size();
			float y_pos = samples[i].first * wave_height + wave_height;
			// if (x == 0)
			// 	wave->moveTo(x_pos, y_pos);
			// else
			// 	wave->lineTo(x_pos, y_pos);
			i = (i + 1) % samples.size();
		}

		// Continue shape, drawing bottom contour right->left
		for (int x = samples.size() - 1; x >= 0; x--) {
			if (--i < 0)
				i = samples.size() - 1;
			float x_pos = (float)x * display_width / (float)samples.size();
			float y_pos = samples[i].second * wave_height + wave_height;
			// wave->lineTo(x_pos, y_pos);
		}

		// wave->strokeWidth(1.0f / scaling);
		// wave->strokeFill(wave_r, wave_g, wave_b, 0xFF);
		// wave->fill(wave_r, wave_g, wave_b, 0x7F);
		// wave->scale(scaling);

		// canvas->update(wave);
		// canvas->draw();
		// canvas->sync();

		return true;
	}

	void hide_graphic_display() {
		// if (canvas) {
		// 	delete canvas;
		// 	canvas = nullptr;
		// }
	}

private:
	// tvg::SwCanvas *canvas = nullptr;
	// tvg::Shape *bar_cursor = nullptr;
	// tvg::Shape *wave = nullptr;

	std::vector<std::pair<float, float>> samples;
	std::atomic<int> newest_sample = 0;

	float cursor_pos = 0;
	float cursor_width = 1;
	float bar_height = 3;

	float x_zoom = 1;
	float x_zoom_ctr = 0;

	uint8_t wave_r = 0, wave_g = 0xFF, wave_b = 0xFF;
	uint8_t bar_r = 0xF0, bar_g = 0x88, bar_b = 0x00;

	float scaling = 1;

	std::span<uint32_t> buffer;
	float oversample_max = 0;
	float oversample_min = 0;

	const float display_width;
	const float display_height;
	const float wave_height;
};
} // namespace MetaModule
