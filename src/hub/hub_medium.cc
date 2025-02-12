#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/hub/HubMedium_info.hh"
#include "flatbuffers/encode.hh"
#include "hub/buttons.hh"
#include "hub/hub_elements.hh"
#include "hub/jack_alias_menu.hh"
#include "hub/knob_set_buttons.hh"
#include "hub_module_widget.hh"
#include "mapping/vcv_patch_file_writer.hh"
#include "network/network.hh"
#include "widgets/config_element.hh"

using namespace rack;

namespace MetaModule
{

struct HubMedium : MetaModuleHubBase {
	using INFO = HubMediumInfo;

	rack::dsp::BooleanTrigger wifibut_trig;
	rack::dsp::BooleanTrigger savebut_trig;

	static constexpr unsigned num_extra_params = 2; //Save Button, Wifi Send Button
	static constexpr auto saveButtonIndex = ElementCount::count<INFO>().num_params;
	static constexpr auto wifiSendButtonIndex = saveButtonIndex + 1;

	HubMedium() {

		// Register with VCV the number of elements of each type
		auto cnt = ElementCount::count<INFO>();
		config(cnt.num_params + num_extra_params, cnt.num_inputs, cnt.num_outputs, cnt.num_lights);

		// hardware io is virtually inverted
		jack_alias.in.resize(cnt.num_outputs);
		jack_alias.out.resize(cnt.num_inputs);

		// Configure elements with VCV
		ConfigElement<INFO> creator{this};
		for (auto &element : INFO::Elements) {
			std::visit([&creator](auto &el) { creator.config_element(el); }, element);
		}

		savebut_trig.reset();
		wifibut_trig.reset();

		configButton(saveButtonIndex, "Save Patch");
		configButton(wifiSendButtonIndex, "Send Patch over Wi-Fi");
	}

	void process(const ProcessArgs &args) override {
		processMaps();

		if (savebut_trig.process(params[saveButtonIndex].getValue() > 0.5f)) {
			should_save = true;
		}

		if (wifibut_trig.process(params[wifiSendButtonIndex].getValue() > 0.5f)) {
			should_send_wifi = true;
		}
	}

private:
	constexpr static auto indices = ElementCount::get_indices<INFO>();

	constexpr static ElementCount::Indices index(INFO::Elem el) {
		auto element_idx = static_cast<std::underlying_type_t<INFO::Elem>>(el);
		return indices[element_idx];
	}
};

std::string wifiUrl = "";
Volume wifiVolume = Volume::Card;

struct HubMediumWidget : MetaModuleHubWidget {

	using INFO = HubMediumInfo;
	static constexpr float kTextOffset = 5;
	static constexpr unsigned kMaxKnobSetNameChars = 16;

	MetaModule::LabelDelay *wifiResponseLabel;

	rack::Label *statusText;
	KnobSetButtonGroup *knobSetButtons;
	TextField *knobSetNameField;

	LedDisplayTextField *patchName;
	LedDisplayTextField *patchDesc;

	HubWifiButton *wifiSendButton;
	MetaModule::LabelOverlay *wifiConnectionLabel;

	HubSaveButton *saveButton;

	std::string lastPatchFilePath;

	std::string wifiConnectionText;

	const std::vector<std::string> volumeLabels = {"Internal", "USB", "Card"};
	const std::vector<std::string> mappingModeLabels = {
		"All", "Left & Right Only", "Right Only", "Left Only", "Connected"};

	HubMediumWidget(HubMedium *module) {
		setModule(module);
		hubModule = module;

		if (hubModule != nullptr) {

			hubModule->updatePatchName = [this] {
				hubModule->patchNameText = patchName->text;
				hubModule->patchDescText = patchDesc->text;
			};

			wifiConnectionText = formatWifiStatus();
		}

		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/HubMedium_artwork.svg")));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(
			rack::math::Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		patchName = createWidget<TextField>(rack::mm2px(rack::math::Vec(36.1, 9.5)));
		if (hubModule != nullptr && hubModule->patchNameText.length() > 0)
			patchName->setText(this->hubModule->patchNameText);
		else
			patchName->setText("Enter Patch Name");
		patchName->placeholder = "Enter Patch Name";
		patchName->color = rack::color::BLACK;
		patchName->box.size = {rack::mm2px(rack::math::Vec(57.7f, 10.0f))};
		patchName->cursor = 0;
		addChild(patchName);

		patchDesc = createWidget<TextField>(rack::mm2px(rack::math::Vec(36.4, 18.f)));
		if (hubModule != nullptr && hubModule->patchDescText.length() > 0)
			patchDesc->setText(this->hubModule->patchDescText);
		else
			patchDesc->setText("Patch Description");
		patchDesc->placeholder = "Patch Description";
		patchDesc->color = rack::color::BLACK;
		patchDesc->box.size = {rack::mm2px(rack::math::Vec(57.7f, 31.3f))};
		patchDesc->cursor = 0;
		// addChild(patchDesc);

		auto knobSetTitle = createWidget<Label>(rack::mm2px(rack::math::Vec(36.4, 50.5)));
		knobSetTitle->color = rack::color::BLACK;
		knobSetTitle->text = "Knob Set:";
		knobSetTitle->fontSize = 10;
		addChild(knobSetTitle);

		knobSetNameField = new TextField{[this](std::string &text) {
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
		HubWidgetCreator<INFO> creator(this, module);
		for (auto &element : INFO::Elements) {
			std::visit([&creator](auto &el) { creator.create(el); }, element);
		}

		// Add the Save button
		saveButton =
			createParamCentered<HubSaveButton>(rack::math::Vec(358.5f, 38.57f), module, HubMedium::saveButtonIndex);
		addParam(saveButton);

		// Wifi
		wifiSendButton = new HubWifiButton();
		wifiSendButton->box.pos = rack::math::Vec(318.f, 38.57f);
		wifiSendButton->app::ParamWidget::module = module;
		wifiSendButton->app::ParamWidget::paramId = HubMedium::wifiSendButtonIndex;
		wifiSendButton->initParamQuantity();
		wifiSendButton->box.pos = wifiSendButton->box.pos.minus(wifiSendButton->box.size.div(2));
		wifiSendButton->getLight()->setBrightnesses({0.5f});
		addParam(wifiSendButton);

		wifiConnectionLabel = new LabelOverlay();
		wifiConnectionLabel->box.pos = rack::mm2px(rack::math::Vec(36.4, 18.f));
		wifiConnectionLabel->text = formatWifiStatus();
		wifiConnectionLabel->color = nvgRGB(0x00, 0x33, 0x66);
		wifiConnectionLabel->box.size = {rack::mm2px(rack::math::Vec(57.7f, 31.3f))};
		wifiConnectionLabel->hide();

		wifiResponseLabel = new LabelDelay(patchName);
		wifiResponseLabel->box.pos = rack::mm2px(rack::math::Vec(36.1, 9.5));
		wifiResponseLabel->box.size = rack::mm2px(rack::math::Vec(57.7f, 10.0f));
		wifiResponseLabel->color = nvgRGB(0x00, 0x33, 0x66);
		wifiResponseLabel->text = "";
		addChild(wifiResponseLabel);

		addChild(wifiConnectionLabel);
		addChild(patchDesc);
		wifiSendButton->setLabels(wifiConnectionLabel, patchDesc);
	}

	std::string cleanupPatchName(std::string patchnm) {
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

		return patchStructName.str;
	}

	void writePatchFile() {
		auto patchStructName = cleanupPatchName(patchName->text);

		osdialog_filters *filters = osdialog_filters_parse("Metamodule Patch File (.yml):yml");
		DEFER({ osdialog_filters_free(filters); });

		std::string dir = lastPatchFilePath;
		if (dir == "")
			dir = rack::asset::userDir.c_str();

		char *filename = osdialog_file(OSDIALOG_SAVE, dir.c_str(), patchStructName.c_str(), filters);
		if (!filename)
			return;

		std::string patchFileName = filename;
		DEFER({ free(filename); });

		if (rack::system::getExtension(rack::system::getFilename(patchFileName)) != ".yml") {
			patchFileName += ".yml";
		}

		lastPatchFilePath = rack::system::getDirectory(patchFileName);

		patchName->text = rack::system::getStem(filename);

		using PatchFileWriter = VCVPatchFileWriter<HubMedium::NumPots, HubMedium::MaxMapsPerPot, MaxKnobSets>;
		auto yml = PatchFileWriter::createPatchYml(
			hubModule->id, hubModule->mappings, hubModule->jack_alias, patchName->text, patchDesc->text, hubModule->mappingMode);
		PatchFileWriter::writeToFile(patchFileName, yml);
	}

	void wifiSendPatchFile() {
		auto patchFileName = cleanupPatchName(patchName->text);
		patchName->text = rack::system::getStem(patchFileName);

		if (rack::system::getExtension(rack::system::getFilename(patchFileName)) != ".yml") {
			patchFileName += ".yml";
		}

		using PatchFileWriter = VCVPatchFileWriter<HubMedium::NumPots, HubMedium::MaxMapsPerPot, MaxKnobSets>;
		auto yml = PatchFileWriter::createPatchYml(
			hubModule->id, hubModule->mappings, hubModule->jack_alias, patchName->text, patchDesc->text, hubModule->mappingMode);
		if (yml.size() > 256 * 1024 && wifiVolume == Volume::Internal) {
			wifiResponseLabel->showFor(180);
			wifiResponseLabel->text = "File too large for Internal: max is 256kB";
			return;
		}
		if (yml.size() > 1024 * 1024) {
			wifiResponseLabel->showFor(180);
			wifiResponseLabel->text = "File too large: max is 1MB";
			return;
		}

		wifiResponseLabel->text = "Sending patch file...";

		std::string vol_string = (size_t)wifiVolume < volumeLabels.size() ? volumeLabels[wifiVolume] : "Card";

		auto encoded = FlatBuffers::encode_file(patchFileName, yml, vol_string);
		auto response_message = network::requestRaw(rack::network::Method::METHOD_POST, wifiUrl + "/action", encoded);
		auto [success, response] = FlatBuffers::decode_response(response_message);

		wifiResponseLabel->showFor(120);
		if (success) {
			wifiResponseLabel->text = "Sent patch file";
		} else {
			wifiResponseLabel->text = "Failed";
		}

		if (response.length())
			wifiResponseLabel->text += ": " + response;
	}

	void promptWifiUrl() {
		auto addr = osdialog_prompt(
			osdialog_message_level::OSDIALOG_INFO, "Enter the address (e.g. http://192.168.1.22)", wifiUrl.c_str());

		if (addr) {
			//TODO: ensure is just http://\d.\d.\d.\d[/]?
			std::string testWifiUrl = addr;

			if (testWifiUrl.starts_with("https://")) {
				testWifiUrl = "http://" + testWifiUrl.substr(8);
			}
			if (!testWifiUrl.starts_with("http://")) {
				testWifiUrl = "http://" + testWifiUrl;
			}
			if (testWifiUrl.length() >= 14) { //smallest IP: http://1.2.3.4 is 14 chars
				wifiUrl = testWifiUrl;
				wifiConnectionLabel->text = formatWifiStatus();
			} else {
				wifiResponseLabel->text = "Not a valid address";
				wifiResponseLabel->showFor(120);
			}
			free(addr);
		}
	}

	void appendContextMenu(rack::Menu *menu) override {
		using namespace rack;
		menu->addChild(new MenuSeparator());
		menu->addChild(createIndexSubmenuItem(
			"Include modules from:",
			mappingModeLabels,
			[this]() { return hubModule->mappingMode; },
			[this](size_t index) { hubModule->setMappingMode(index); }));
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuItem("Set Wi-Fi Expander address", "", [this]() { promptWifiUrl(); }));
		menu->addChild(createIndexSubmenuItem(
			"Wi-Fi sends to:",
			volumeLabels,
			[]() { return wifiVolume; },
			[this](size_t index) {
				wifiVolume = Volume(index);
				wifiConnectionLabel->text = formatWifiStatus();
			}));

		menu->addChild(new MenuSeparator());

		auto jacks = std::array{
			std::make_tuple(std::span{hubModule->jack_alias.in.begin(), PanelDef::NumAudioIn}, "Input "),
			std::make_tuple(std::span{hubModule->jack_alias.in.begin() + PanelDef::NumAudioIn, PanelDef::NumGateIn},
							"Gate "),
			std::make_tuple(std::span{hubModule->jack_alias.out}, "Output ")};
		menu->addChild(createAliasSubmenu(jacks));

		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel<MenuLabel>("Mapped Knob Sets"));

		for (unsigned knobset_idx = 0; knobset_idx < hubModule->MaxKnobSets; knobset_idx++) {
			menu->addChild(new MenuSeparator());
			menu->addChild(createCheckMenuItem(
				string::f("Knob Set %d", knobset_idx + 1),
				"",
				[=, this]() { return hubModule->mappings.getActiveKnobSetIdx() == knobset_idx; },
				[=, this]() {
					hubModule->mappings.changeActiveKnobSet(knobset_idx, ShouldLock::Yes);
					updateKnobSetLabel();
				}));

			menu->addChild(new KnobSetNameMenuItem{[this](unsigned idx, std::string const &text) {
													   hubModule->mappings.setKnobSetName(idx, text);
													   updateKnobSetLabel();
												   },
												   knobset_idx,
												   hubModule->mappings.getKnobSetName(knobset_idx),
												   kMaxKnobSetNameChars});
		}
	}

	std::string formatWifiStatus() {
		return "Wi-Fi Connection:\n" + wifiUrl + "\n" +
			   "Copy patch to: " + (size_t(wifiVolume) < volumeLabels.size() ? volumeLabels[wifiVolume] : "");
	}

	void updateKnobSetLabel() {
		if (!hubModule || !knobSetNameField || !knobSetButtons)
			return;

		auto activeKnobSetIdx = hubModule->mappings.getActiveKnobSetIdx();
		if (auto name = hubModule->mappings.getActiveKnobSetName(); name.length() > 0) {
			knobSetNameField->text = name;
		} else {
			knobSetNameField->text = rack::string::f("Knob Set %d", activeKnobSetIdx + 1);
		}

		knobSetButtons->active_idx = activeKnobSetIdx;
	}

	void step() override {
		if (wifiUrl.length() > 13)
			wifiSendButton->getLight()->setBrightnesses({0.50f});
		else
			wifiSendButton->getLight()->setBrightnesses({0.f});

		if (hubModule) {
			if (hubModule->should_save) {
				hubModule->should_save = false;
				writePatchFile();
			}

			if (hubModule->should_send_wifi) {
				hubModule->should_send_wifi = false;
				if (wifiUrl.length()) {
					wifiSendPatchFile();
				} else {
					promptWifiUrl();
					if (wifiUrl.length()) {
						wifiSendPatchFile();
					}
				}
			}
		}
		MetaModuleHubWidget::step();
	}
};

} // namespace MetaModule

rack::Model *modelHubMedium = rack::createModel<MetaModule::HubMedium, MetaModule::HubMediumWidget>("HubMedium");
