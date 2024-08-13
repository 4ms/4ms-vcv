#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/hub/HubMedium_info.hh"
#include "CoreModules/moduleFactory.hh"
#include "comm/comm_module.hh"
#include "hub/hub_elements.hh"
#include "hub/knob_set_buttons.hh"
#include "hub_module_widget.hh"
#include "mapping/patch_writer.hh"
#include "widgets/4ms/4ms_widgets.hh"
#include "widgets/vcv_module_creator.hh"
#include "widgets/vcv_widget_creator.hh"

using namespace rack;

struct HubMedium : MetaModuleHubBase {
	using INFO = MetaModule::HubMediumInfo;

	HubMedium() {

		// Register with VCV the number of elements of each type
		auto cnt = ElementCount::count<INFO>();
		config(cnt.num_params, cnt.num_inputs, cnt.num_outputs, cnt.num_lights);

		// Configure elements with VCV
		MetaModule::VCVModuleParamCreator<INFO> creator{this};
		for (auto &element : INFO::Elements) {
			std::visit([&creator](auto &el) { creator.config_element(el); }, element);
		}
	}

	void process(const ProcessArgs &args) override {
		processPatchButton(params[save_patch_button_idx].getValue());
		processMaps();
	}

private:
	constexpr static auto indices = ElementCount::get_indices<INFO>();

	constexpr static ElementCount::Indices index(INFO::Elem el) {
		auto element_idx = static_cast<std::underlying_type_t<INFO::Elem>>(el);
		return indices[element_idx];
	}

	unsigned save_patch_button_idx = index(INFO::Elem::SavepatchButton).param_idx;
};

struct HubMediumWidget : MetaModuleHubWidget {
	using INFO = MetaModule::HubMediumInfo;

	LedDisplayTextField *patchName;
	LedDisplayTextField *patchDesc;

	HubMediumWidget(HubMedium *module) {
		setModule(module);
		hubModule = module;

		if (hubModule != nullptr) {
			hubModule->updateDisplay = [this] {
				statusText->text = hubModule->labelText;
			};

			hubModule->updatePatchName = [this] {
				hubModule->patchNameText = patchName->text;
				hubModule->patchDescText = patchDesc->text;
			};
		}

		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/HubMedium_artwork.svg")));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(
			rack::math::Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		patchName = createWidget<MetaModule::MetaModuleTextBox>(rack::mm2px(rack::math::Vec(36.1, 9.5)));
		if (hubModule != nullptr && hubModule->patchNameText.length() > 0)
			patchName->setText(this->hubModule->patchNameText);
		else
			patchName->setText("Enter Patch Name");
		patchName->placeholder = "Enter Patch Name";
		patchName->color = rack::color::BLACK;
		patchName->box.size = {rack::mm2px(rack::math::Vec(57.7f, 10.0f))};
		patchName->cursor = 0;
		addChild(patchName);

		statusText = createWidget<Label>(rack::mm2px(rack::math::Vec(10, 1.5)));
		statusText->color = rack::color::WHITE;
		statusText->text = "";
		statusText->fontSize = 10;
		addChild(statusText);

		patchDesc = createWidget<MetaModule::MetaModuleTextBox>(rack::mm2px(rack::math::Vec(36.4, 18.f)));
		if (hubModule != nullptr && hubModule->patchDescText.length() > 0)
			patchDesc->setText(this->hubModule->patchDescText);
		else
			patchDesc->setText("Patch Description");
		patchDesc->placeholder = "Patch Description";
		patchDesc->color = rack::color::BLACK;
		patchDesc->box.size = {rack::mm2px(rack::math::Vec(57.7f, 31.3f))};
		patchDesc->cursor = 0;
		addChild(patchDesc);

		auto knobSetTitle = createWidget<Label>(rack::mm2px(rack::math::Vec(36.4, 50.5)));
		knobSetTitle->color = rack::color::BLACK;
		knobSetTitle->text = "Knob Set:";
		knobSetTitle->fontSize = 10;
		addChild(knobSetTitle);

		knobSetNameField = new MetaModuleTextField{[this](std::string &text) {
													   auto idx = hubModule->mappings.getActiveKnobSetIdx();
													   hubModule->mappings.setKnobSetName(idx, text);
												   },
												   kMaxKnobSetNameChars};
		knobSetNameField->box.pos = rack::mm2px(rack::math::Vec(52.0, 49.0));
		knobSetNameField->box.size = {rack::mm2px(rack::math::Vec(40.f, 7.f))};
		knobSetNameField->text = "";
		knobSetNameField->color = rack::color::BLACK;
		knobSetNameField->cursor = 0;
		addChild(knobSetNameField);

		knobSetButtons = new KnobSetButtonGroup(
			[this](unsigned idx) {
				hubModule->mappings.setActiveKnobSetIdx(idx);
				updateKnobSetLabel();
			},
			rack::mm2px(rack::Vec(39.5, 57.5)));
		addChild(knobSetButtons);
		updateKnobSetLabel();

		// create widgets from all elements
		MetaModule::HubWidgetCreator<INFO> creator(this, module);
		for (auto &element : INFO::Elements) {
			std::visit([&creator](auto &el) { creator.create(el); }, element);
		}
	}
};

rack::Model *modelHubMedium = rack::createModel<HubMedium, HubMediumWidget>("HubMedium");
