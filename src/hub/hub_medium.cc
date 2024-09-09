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

	static constexpr unsigned num_extra_params = 1; //Save Button

	HubMedium() {

		// Register with VCV the number of elements of each type
		auto cnt = ElementCount::count<INFO>();
		config(cnt.num_params + num_extra_params, cnt.num_inputs, cnt.num_outputs, cnt.num_lights);

		// Configure elements with VCV
		MetaModule::VCVModuleParamCreator<INFO> creator{this};
		for (auto &element : INFO::Elements) {
			std::visit([&creator](auto &el) { creator.config_element(el); }, element);
		}

		constexpr auto saveButtonIndex = ElementCount::count<INFO>().num_params;
		configParam(saveButtonIndex, 0, 1, 0, "Save Patch");
	}

	void process(const ProcessArgs &args) override {
		processMaps();
	}

private:
	constexpr static auto indices = ElementCount::get_indices<INFO>();

	constexpr static ElementCount::Indices index(INFO::Elem el) {
		auto element_idx = static_cast<std::underlying_type_t<INFO::Elem>>(el);
		return indices[element_idx];
	}
};

struct HubMediumWidget : MetaModuleHubWidget {

	using INFO = MetaModule::HubMediumInfo;

	LedDisplayTextField *patchName;
	LedDisplayTextField *patchDesc;

	std::string lastPatchFilePath;

	HubMediumWidget(HubMedium *module) {
		setModule(module);
		hubModule = module;

		if (hubModule != nullptr) {
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

		patchName = createWidget<MetaModule::TextField>(rack::mm2px(rack::math::Vec(36.1, 9.5)));
		if (hubModule != nullptr && hubModule->patchNameText.length() > 0)
			patchName->setText(this->hubModule->patchNameText);
		else
			patchName->setText("Enter Patch Name");
		patchName->placeholder = "Enter Patch Name";
		patchName->color = rack::color::BLACK;
		patchName->box.size = {rack::mm2px(rack::math::Vec(57.7f, 10.0f))};
		patchName->cursor = 0;
		addChild(patchName);

#ifdef DEBUG_STATUS_LABEL
		statusText = createWidget<Label>(rack::mm2px(rack::math::Vec(10, 1.5)));
		statusText->color = rack::color::WHITE;
		statusText->text = "";
		statusText->fontSize = 10;
		addChild(statusText);
#endif

		patchDesc = createWidget<MetaModule::TextField>(rack::mm2px(rack::math::Vec(36.4, 18.f)));
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

		knobSetNameField = new MetaModule::TextField{[this](std::string &text) {
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
				hubModule->mappings.changeActiveKnobSet(idx, ShouldLock::Yes);
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

		// Add the Save button
		constexpr auto saveButtonIndex = ElementCount::count<INFO>().num_params;
		auto saveButton = createParamCentered<HubSaveButton>(rack::math::Vec(358.5f, 38.57f), module, saveButtonIndex);
		saveButton->click_callback = [this]() {
			writePatchFile();
		};
		addParam(saveButton);
	}

	void writePatchFile() {
		std::string patchnm = patchName->text;

		if (patchnm == "" || patchnm == "Enter Patch Name") {
			patchnm = "Untitled Patch " + std::to_string(MathTools::randomNumber<unsigned int>(100, 999));
		}

		ReplaceString patchStructName{patchnm};
		// Remove slashes, colons, quotes, and replace dots with _
		patchStructName.replace_all("/", "")
			.replace_all("\\", "")
			.replace_all("\"", "")
			.replace_all("'", "")
			.replace_all(":", "")
			.replace_all(".", "_");

		osdialog_filters *filters = osdialog_filters_parse("Metamodule Patch File (.yml):yml");
		DEFER({ osdialog_filters_free(filters); });

		std::string dir = lastPatchFilePath;
		if (dir == "")
			dir = rack::asset::userDir.c_str();

		char *filename = osdialog_file(OSDIALOG_SAVE, dir.c_str(), patchStructName.str.c_str(), filters);
		if (!filename)
			return;

		std::string patchFileName = filename;
		DEFER({ free(filename); });

		if (rack::system::getExtension(rack::system::getFilename(patchFileName)) != ".yml") {
			patchFileName += ".yml";
		}

		lastPatchFilePath = rack::system::getDirectory(patchFileName);

#ifdef DEBUG_STATUS_LABEL
		statusText->text = "Creating patch...";
#endif

		patchName->text = rack::system::getStem(filename);

		VCVPatchFileWriter<HubMedium::NumPots, HubMedium::MaxMapsPerPot, MaxKnobSets>::writePatchFile(
			hubModule->id, hubModule->mappings, patchFileName, patchName->text, patchDesc->text);

#ifdef DEBUG_STATUS_LABEL
		statusText->text = "Wrote patch file";
		statusText->text += rack::system::getFilename(patchFileName);

		statusText->text = "";
#endif
	}

	struct HubSaveButton : rack::BefacoPush {
		std::function<void(void)> click_callback;

		void onDragEnd(const rack::event::DragEnd &e) override {
			OpaqueWidget::onDragEnd(e);
			if (click_callback) {
				click_callback();
			}
		}
	};
};

rack::Model *modelHubMedium = rack::createModel<HubMedium, HubMediumWidget>("HubMedium");
