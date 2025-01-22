#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/hub/MMAIO_info.hh"
#include "hub/hub_elements.hh"
#include "widgets/4ms/4ms_widgets.hh"
#include "widgets/vcv_module_creator.hh"

namespace MetaModule
{

struct AudioExpander : MetaModuleHubBase {
	using Info = MMAIOInfo;
	AudioExpander() {
		// Register with VCV the number of elements of each type
		auto cnt = ElementCount::count<Info>();
		config(cnt.num_params, cnt.num_inputs, cnt.num_outputs, cnt.num_lights);

		// Configure elements with VCV
		VCVModuleParamCreator<Info> creator{this};
		for (auto &element : Info::Elements) {
			std::visit([&creator](auto &el) { creator.config_element(el); }, element);
		}
	}

	void process(const ProcessArgs &args) override {
	}
};

struct AudioExpanderWidget : MetaModuleHubWidget {

	AudioExpanderWidget(rack::Module *module) {
		using namespace rack;

		setModule(module);
		setPanel(APP->window->loadSvg(rack::asset::plugin(pluginInstance, std::string{MMAIOInfo::svg_filename})));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		HubWidgetCreator<MMAIOInfo> creator(this, module);
		for (auto &element : MMAIOInfo::Elements) {
			std::visit([&creator](auto &el) { creator.create(el); }, element);
		}
	}
};

} // namespace MetaModule

rack::Model *modelMMAudioExpander =
	rack::createModel<MetaModule::AudioExpander, MetaModule::AudioExpanderWidget>("MMAudioExpander");
