#include "graphics/waveform_display.hh"
#include "CoreModules/CoreProcessor.hh"
#include "CoreModules/elements/units.hh"
#include "nanovg.h"
#include <algorithm>
#include <atomic>
#include <cmath>

namespace MetaModule
{

struct StreamingWaveformDisplay::Internal {
	NVGcontext *vg = nullptr;
	NVGcontext **vgaddr = nullptr;
};

StreamingWaveformDisplay::StreamingWaveformDisplay(float display_width_mm, float display_height_mm)
	: internal{new Internal}
	, bar_height{5}
	, display_width{mm_to_px(display_width_mm, 379)}
	, display_height{mm_to_px(display_height_mm, 379)}
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

void StreamingWaveformDisplay::set_wave_color(std::span<const float, 3> rgb) {
	set_wave_color(255.f * rgb[0], 255.f * rgb[1], 255.f * rgb[2]);
}

void StreamingWaveformDisplay::set_bar_bg_color(uint8_t r, uint8_t g, uint8_t b) {
	bar_r = r;
	bar_g = g;
	bar_b = b;
}

void StreamingWaveformDisplay::set_bar_bg_color(std::span<const float, 3> rgb) {
	set_bar_bg_color(255.f * rgb[0], 255.f * rgb[1], 255.f * rgb[2]);
}

void StreamingWaveformDisplay::set_bar_fg_color(uint8_t r, uint8_t g, uint8_t b) {
	hilite_r = r;
	hilite_g = g;
	hilite_b = b;
}

void StreamingWaveformDisplay::set_bar_fg_color(std::span<const float, 3> rgb) {
	set_bar_fg_color(255.f * rgb[0], 255.f * rgb[1], 255.f * rgb[2]);
}

void StreamingWaveformDisplay::set_cursor_width(unsigned width) {
	cursor_width = width;
}

void StreamingWaveformDisplay::set_bar_begin_end(float begin, float end) {
	highlight_begin = begin;
	highlight_end = end;
}

// Functions below here run in the GUI thread and may get interrupted by the functions above

void StreamingWaveformDisplay::show_graphic_display(std::span<uint32_t> pix_buffer,
													unsigned width,
													lv_obj_t *context_ptr) {
	internal->vgaddr = reinterpret_cast<NVGcontext **>(context_ptr);
}

bool StreamingWaveformDisplay::draw_graphic_display() {
	if (!internal->vgaddr)
		return false;

	internal->vg = *internal->vgaddr;
	if (!internal->vg)
		return false;

	// Bar
	nvgBeginPath(internal->vg);
	nvgRect(internal->vg, 0, display_height - bar_height / scaling, display_width, bar_height / scaling);
	nvgFillColor(internal->vg, nvgRGBA(bar_r, bar_g, bar_b, 0xFF));
	nvgFill(internal->vg);
	nvgClosePath(internal->vg);

	nvgBeginPath(internal->vg);
	if (highlight_begin <= highlight_end) {
		nvgRect(internal->vg,
				highlight_begin * display_width,
				display_height - bar_height / scaling,
				(highlight_end - highlight_begin) * display_width,
				bar_height / scaling);
	} else {
		// [0, end]
		nvgRect(internal->vg,
				0,
				display_height - bar_height / scaling,
				highlight_end * display_width,
				bar_height / scaling);
		// [begin, max]
		nvgRect(internal->vg,
				highlight_begin * display_width,
				display_height - bar_height / scaling,
				(1.f - highlight_begin) * display_width,
				bar_height / scaling);
	}
	nvgFillColor(internal->vg, nvgRGBA(hilite_r, hilite_g, hilite_b, 0xFF));
	nvgFill(internal->vg);
	nvgClosePath(internal->vg);

	// Trolley
	nvgBeginPath(internal->vg);
	auto cursor_x = cursor_pos * (display_width - cursor_width);
	nvgRect(internal->vg, cursor_x, display_height - bar_height, cursor_width, bar_height);
	nvgFillColor(internal->vg, nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF));
	nvgFill(internal->vg);
	nvgClosePath(internal->vg);

	// Waveform
	nvgScissor(internal->vg, 0, 0, display_width, display_height - bar_height);
	nvgBeginPath(internal->vg);

	//start with the oldest sample
	int start = newest_sample.load() + 1;

	// Draw top contour left->right
	int i = start;
	for (auto x = 0u; x < samples.size(); x++) {
		float x_pos = (float)x * display_width / (float)samples.size();
		float y_pos = samples[i].first * wave_height + wave_height;
		if (x == 0)
			nvgMoveTo(internal->vg, x_pos, y_pos);
		else
			nvgLineTo(internal->vg, x_pos, y_pos);
		i = (i + 1) % samples.size();
	}

	// Continue shape, drawing bottom contour right->left
	for (int x = samples.size() - 1; x >= 0; x--) {
		if (--i < 0)
			i = samples.size() - 1;
		float x_pos = (float)x * display_width / (float)samples.size();
		float y_pos = samples[i].second * wave_height + wave_height;
		nvgLineTo(internal->vg, x_pos, y_pos);
	}
	nvgClosePath(internal->vg);

	nvgStrokeColor(internal->vg, nvgRGBA(wave_r, wave_g, wave_b, 0xFF));
	nvgFillColor(internal->vg, nvgRGBA(wave_r, wave_g, wave_b, 0x7F));
	nvgStrokeWidth(internal->vg, 0.5f);
	nvgFill(internal->vg);
	nvgStroke(internal->vg);

	nvgResetScissor(internal->vg);

	return true;
}

void StreamingWaveformDisplay::hide_graphic_display() {
}

} // namespace MetaModule
