#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/hub/MMAudioExpander_info.hh"
#include "hub/hub_elements.hh"
#include "hub/jack_alias.hh"
#include "hub/jack_alias_menu.hh"
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

		// hardware io is virtually inverted
		jack_alias.in.resize(cnt.num_outputs);
		jack_alias.out.resize(cnt.num_inputs);

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

	AudioExpanderWidget(AudioExpanderModule *module) {
		using namespace rack;

		setModule(module);
		hubModule = module;
		setPanel(APP->window->loadSvg(rack::asset::plugin(pluginInstance, std::string{Info::svg_filename})));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		HubWidgetCreator<Info> creator(this, module);
		for (auto &element : Info::Elements) {
			std::visit([&creator](auto &el) { creator.create(el); }, element);
		}
	}

	void appendContextMenu(rack::Menu *menu) override {
		using namespace rack;
		menu->addChild(new MenuSeparator());

		auto jacks =
			std::vector<JackMenuCategories>{{"Input ", MetaModuleHubBase::JackDir::In, {0, 1, 2, 3, 4, 5}},
											{"Output ", MetaModuleHubBase::JackDir::Out, {0, 1, 2, 3, 4, 5, 6, 7}}};
		menu->addChild(createAliasSubmenu(jacks));
	}
};

} // namespace MetaModule

rack::Model *modelMMAudioExpander =
	rack::createModel<MetaModule::AudioExpanderModule, MetaModule::AudioExpanderWidget>("MMAudioExpander");
