#include "button_expander.hh"
#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/hub/MMButtonExpander_info.hh"
#include "hub/hub_elements.hh"
#include "widgets/config_element.hh"

namespace MetaModule
{

ButtonExpanderModule::ButtonExpanderModule()
	: MetaModuleHubBase(8) {

	using Info = MMButtonExpanderInfo;

	// Register with VCV the number of elements of each type
	auto cnt = ElementCount::count<Info>();
	config(cnt.num_params, cnt.num_inputs, cnt.num_outputs, cnt.num_lights);

	// Configure elements with VCV
	ConfigElement<Info> creator{this};
	for (auto &element : Info::Elements) {
		std::visit([&creator](auto &el) { creator.config_element(el); }, element);
	}
}

static bool checkUniqueAddress(unsigned addr, int64_t thisModuleId) {
	auto context = rack::contextGet();
	auto engine = context->engine;
	auto moduleIDs = engine->getModuleIds();

	// Scan patch for all ButtonExpanders and mark which IDs are already used
	for (auto moduleID : moduleIDs) {

		auto *module = engine->getModule(moduleID);

		if (auto *button_exp = dynamic_cast<ButtonExpanderModule *>(module)) {
			if (moduleID != thisModuleId && button_exp->buttonExpanderId == addr)
				return false;
		}
	}
	return true;
}

void ButtonExpanderModule::process(const ProcessArgs &args) {
	processMaps();
}

ButtonExpanderWidget::ButtonExpanderWidget(ButtonExpanderModule *module)
	: MetaModuleHubWidget{module}
	, id_label{nullptr}
	, buttonExpModule{module} {

	using namespace rack;
	using Info = MMButtonExpanderInfo;

	setModule(module);
	setPanel(APP->window->loadSvg(rack::asset::plugin(pluginInstance, std::string{Info::svg_filename})));
	addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	id_label = createWidget<rack::Label>(rack::math::Vec(55, RACK_GRID_HEIGHT - 30));
	id_label->color = nvgRGB(0xFF, 0x8F, 0x8F);
	id_label->text = "";
	id_label->fontSize = 11;
	addChild(id_label);

	HubWidgetCreator<Info> creator(this, module);
	for (auto &element : Info::Elements) {
		std::visit([&creator](auto &el) { creator.create(el); }, element);
	}
}

void ButtonExpanderWidget::step() {
	if (id_label && buttonExpModule) {
		id_label->text = "#" + std::to_string(buttonExpModule->buttonExpanderId + 1);

		if (!checkUniqueAddress(buttonExpModule->buttonExpanderId, buttonExpModule->id)) {
			id_label->color = nvgRGB(0xFF, 0, 0);
		} else {
			id_label->color = nvgRGB(0x8F, 0x8F, 0xFF);
		}
	}

	MetaModuleHubWidget::step();
}

void ButtonExpanderWidget::appendContextMenu(rack::Menu *menu) {
	using namespace rack;

	menu->addChild(new MenuSeparator());
	menu->addChild(createIndexPtrSubmenuItem("Address:", {"1", "2", "3", "4"}, &buttonExpModule->buttonExpanderId));
	menu->addChild(new MenuSeparator());

	MetaModuleHubWidget::appendContextMenu(menu);
}

} // namespace MetaModule

rack::Model *modelMMButtonExpander =
	rack::createModel<MetaModule::ButtonExpanderModule, MetaModule::ButtonExpanderWidget>("MMButtonExpander");
