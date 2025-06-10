#include "hub_jack_label.hh"

namespace MetaModule
{

void HubJackLabel::draw(const DrawArgs &args) {
	if (!hub)
		return;

	auto const &names = dir == JackDir::Output ? hub->jack_alias.in : hub->jack_alias.out;
	if (jack_id < 0 || jack_id >= (int)names.size())
		return;

	auto const &name = names[jack_id];

	if (name.length()) {
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, nvgRGB(0x23, 0x1f, 0x20));
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgTextAlign(args.vg, NVGalign::NVG_ALIGN_CENTER | NVGalign::NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGB(0xbb, 0xbb, 0xbb));
		nvgFontSize(args.vg, 9.0f);
		nvgText(args.vg, box.size.x / 2.f, box.size.y / 2.f, name.c_str(), NULL);
	}
}

} // namespace MetaModule
