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

		float bounds[4];
		nvgTextAlign(args.vg, NVGalign::NVG_ALIGN_CENTER | NVGalign::NVG_ALIGN_MIDDLE);
		nvgFontSize(args.vg, 7.5f);
		nvgTextBounds(args.vg, box.size.x / 2.f, box.size.y / 2.f, name.c_str(), NULL, bounds);

		nvgFillColor(args.vg, nvgRGBA(0x23, 0x1f, 0x20, 0xff));
		nvgRect(args.vg, bounds[0] - 2, bounds[1] - 2, bounds[2] + 2, bounds[3] + 2);
		nvgFill(args.vg);

		nvgTextAlign(args.vg, NVGalign::NVG_ALIGN_CENTER | NVGalign::NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
		nvgText(args.vg, box.size.x / 2.f, box.size.y / 2.f, name.c_str(), NULL);
	}
}

} // namespace MetaModule
