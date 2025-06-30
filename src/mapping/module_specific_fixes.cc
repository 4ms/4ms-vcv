#include "module_specific_fixes.hh"
#include "cable_color.hh"
#include <rack.hpp>

namespace MetaModule
{

bool apply_module_specific_fixes(rack::engine::Cable *cable, std::vector<CableMap> &cableData) {
	if (!cable || !cable->inputModule || !cable->outputModule)
		return false;

	auto outModel = cable->outputModule->getModel();
	auto inModel = cable->inputModule->getModel();

	if (!outModel || !inModel)
		return false;

	{
		constexpr int Prism_POLY_OUT = 0;
		constexpr int Prism_MONO_OUT_1 = 16;

		if (outModel->getFullName() == "Prism Rainbow" && cable->outputId == Prism_POLY_OUT && inModel->slug == "Split")
		{
			// Replace this:
			//    Prism Rainbow Poly Output --> Split --> multiple mono cables to various modules
			// With this:
			//    Prism Rainbow Mono Outputs 1-6 --> various modules

			// First, find cables on the outputs of the Split module:
			auto *prismModule = cable->outputModule;
			auto *splitModule = cable->inputModule;

			auto cables = APP->engine->getCableIds();
			for (auto cableID : cables) {
				if (auto *c = APP->engine->getCable(cableID); c->outputModule == splitModule) {

					// Create a cable from Prism MONO output 0-5 to the other end of the cables on Split outputs 0-5
					if (c->outputId < 6) {
						cableData.push_back({
							.outputJackId = Prism_MONO_OUT_1 + c->outputId,
							.inputJackId = c->inputId,
							.outputModuleId = prismModule->getId(),
							.inputModuleId = c->inputModule->getId(),
							.lv_color_full = cable_color_rgb565(APP->scene->rack->getCable(cableID)),
						});
					}
				}
			}

			return true; //handled
		}
	}

	{
		constexpr int Prism_POLY_IN = 12;
		constexpr int Prism_MONO_IN_1 = 27;

		if (inModel->getFullName() == "Prism Rainbow" && cable->inputId == Prism_POLY_IN &&
			outModel->getFullName() == "VCV Merge")
		{
			// Replace this:
			//    various modules --> Merge --> Prism Rainbow Poly Input
			// With this:
			//    various modules --> Prism Rainbow Mono Inputs 1-6

			// First, find cables on the inputs of the Merge module:
			auto *prismModule = cable->inputModule;
			auto *mergeModule = cable->outputModule;

			auto cables = APP->engine->getCableIds();
			for (auto cableID : cables) {
				if (auto *c = APP->engine->getCable(cableID); c->inputModule == mergeModule) {

					// Create a cable from the other end of the cable on Merge inputs 0-5 to Prism MONO inputs 0-5
					if (c->inputId < 6) {
						cableData.push_back({
							.outputJackId = c->outputId,
							.inputJackId = Prism_MONO_IN_1 + c->inputId,
							.outputModuleId = c->outputModule->getId(),
							.inputModuleId = prismModule->getId(),
							.lv_color_full = cable_color_rgb565(APP->scene->rack->getCable(cableID)),
						});
					}
				}
			}

			return true; //handled
		}
	}

	return false; // not handled
}

} // namespace MetaModule
