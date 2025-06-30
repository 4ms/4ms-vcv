#include "module_specific_fixes.hh"
#include <rack.hpp>

namespace MetaModule
{

bool apply_module_specific_fixes(uint16_t color, rack::engine::Cable *cable, std::vector<CableMap> &cableData) {
	if (!cable || !cable->inputModule || !cable->outputModule)
		return false;

	auto outModel = cable->outputModule->getModel();
	auto inModel = cable->inputModule->getModel();

	if (!outModel || !inModel)
		return false;

	constexpr int Prism_POLY_OUT_OUTPUT = 0;
	if (outModel->getFullName() == "Prism Rainbow" && cable->outputId == Prism_POLY_OUT_OUTPUT) {
		// POLY_OUT_OUTPUT -> Split module
		if (inModel->slug == "Split") {

			// find where the split outputs are routed, and make cables directly from Prism to those locations
			for (auto cableID : APP->engine->getCableIds()) {
				if (auto *c = APP->engine->getCable(cableID); c->outputModule == cable->inputModule) {
					cableData.push_back({
						.outputJackId = cable->outputId, //16-up
						.inputJackId = c->inputId,
						.outputModuleId = cable->outputModule->getId(),
						.inputModuleId = c->inputModule->getId(),
						.lv_color_full = color,
					});
				}
			}
		}
	}

	if (inModel->getFullName() == "Prism Rainbow" && cable->inputId == 12) {
		// POLY_IN_INPUT
	}

	return true; //handled
}

} // namespace MetaModule
