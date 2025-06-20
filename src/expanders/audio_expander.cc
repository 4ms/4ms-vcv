#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/hub/MMAudioExpander_info.hh"
#include "hub/hub_elements.hh"
#include "widgets/4ms/4ms_widgets.hh"
#include "widgets/config_element.hh"

namespace MetaModule
{

struct AudioExpanderModule : MetaModuleHubBase {
	using Info = MMAudioExpanderInfo;
	AudioExpanderModule() {
		// Register with VCV the number of elements of each type
		auto cnt = ElementCount::count<Info>();
		config(cnt.num_params, cnt.num_inputs, cnt.num_outputs, cnt.num_lights);

		// Configure elements with VCV
		ConfigElement<Info> creator{this};
		for (auto &element : Info::Elements) {
			std::visit([&creator](auto &el) { creator.config_element(el); }, element);
		}
	}

	void process(const ProcessArgs &args) override {
	}
};

struct AudioExpanderWidget : MetaModuleHubWidget {
	using Info = AudioExpanderModule::Info;

	AudioExpanderWidget(rack::Module *module) {
		using namespace rack;

		setModule(module);
		setPanel(APP->window->loadSvg(rack::asset::plugin(pluginInstance, std::string{Info::svg_filename})));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		HubWidgetCreator<Info> creator(this, module);
		for (auto &element : Info::Elements) {
			std::visit([&creator](auto &el) { creator.create(el); }, element);
		}
	}
};

} // namespace MetaModule

rack::Model *modelMMAudioExpander =
	rack::createModel<MetaModule::AudioExpanderModule, MetaModule::AudioExpanderWidget>("MMAudioExpander");
