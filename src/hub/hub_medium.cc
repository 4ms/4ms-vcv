#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/hub/HubMedium_info.hh"
#include "CoreModules/moduleFactory.hh"
#include "comm/comm_module.hh"
#include "flatbuffers/encode.hh"
#include "hub/buttons.hh"
#include "hub/hub_elements.hh"
#include "hub/knob_set_buttons.hh"
#include "hub/label_delay.hh"
#include "hub_module_widget.hh"
#include "mapping/patch_writer.hh"
#include "network/network.hh"
#include "widgets/4ms/4ms_widgets.hh"
#include "widgets/vcv_module_creator.hh"
#include "widgets/vcv_widget_creator.hh"

using namespace rack;

namespace MetaModule
{

struct HubMedium : MetaModuleHubBase {
	using INFO = HubMediumInfo;

	static constexpr unsigned num_extra_params = 2; //Save Button, Wifi Send Button
	static constexpr auto saveButtonIndex = ElementCount::count<INFO>().num_params;
	static constexpr auto wifiSendButtonIndex = saveButtonIndex + 1;

	HubMedium() {

		// Register with VCV the number of elements of each type
		auto cnt = ElementCount::count<INFO>();
		config(cnt.num_params + num_extra_params, cnt.num_inputs, cnt.num_outputs, cnt.num_lights);

		// Configure elements with VCV
		VCVModuleParamCreator<INFO> creator{this};
		for (auto &element : INFO::Elements) {
			std::visit([&creator](auto &el) { creator.config_element(el); }, element);
		}

		configParam(saveButtonIndex, 0, 1, 0, "Save Patch");
		configParam(wifiSendButtonIndex, 0, 1, 0, "Send Patch over Wi-Fi");
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

	using INFO = HubMediumInfo;

	MetaModule::LabelDelay *statusText;
	rack::Label *wifiURLText;

	KnobSetButtonGroup *knobSetButtons;
	TextField *knobSetNameField;
	LedDisplayTextField *patchName;
	LedDisplayTextField *patchDesc;
	HubWifiButton *wifiSendButton;

	std::string lastPatchFilePath;

	std::string wifiUrl = "";
	enum Volume { Internal = 0, USB = 1, Card = 2 } wifiVolume = Volume::Card;
	const std::vector<std::string> volumeLabels = {"Internal", "USB", "Card"};

	static constexpr unsigned kMaxKnobSetNameChars = 16;

	HubMediumWidget(HubMedium *module) {
		setModule(module);
		hubModule = module;

		if (hubModule != nullptr) {

			hubModule->updatePatchName = [this] {
				hubModule->patchNameText = patchName->text;
				hubModule->patchDescText = patchDesc->text;
				hubModule->wifiUrl = wifiUrl;
				hubModule->wifiPath = size_t(wifiVolume) < volumeLabels.size() ? volumeLabels[wifiVolume] : "Card";
			};

			wifiUrl = hubModule->wifiUrl;
			wifiVolume = hubModule->wifiPath == "USB"	   ? Volume::USB :
						 hubModule->wifiPath == "Card"	   ? Volume::Card :
						 hubModule->wifiPath == "Internal" ? Volume::Internal :
															 Volume::Card;
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

		statusText = createWidget<LabelDelay>(rack::mm2px(rack::math::Vec(10, 1.5)));
		statusText->box.size = rack::mm2px(rack::math::Vec(200, 20));
		statusText->color = rack::color::WHITE;
		statusText->text = "";
		statusText->fontSize = 10;
		addChild(statusText);

		patchDesc = createWidget<TextField>(rack::mm2px(rack::math::Vec(36.4, 18.f)));
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
		auto saveButton =
			createParamCentered<HubSaveButton>(rack::math::Vec(358.5f, 38.57f), module, HubMedium::saveButtonIndex);
		saveButton->click_callback = [this]() {
			writePatchFile();
		};
		addParam(saveButton);

		// Wifi
		wifiURLText = createWidget<Label>(rack::math::Vec(260, 5));
		wifiURLText->box.size = rack::mm2px(rack::math::Vec(120, 20));
		wifiURLText->color = rack::color::WHITE;
		wifiURLText->text = formatWifiStatus();
		wifiURLText->fontSize = 9;
		addChild(wifiURLText);
		wifiURLText->hide();

		wifiSendButton = new HubWifiButton(wifiURLText);
		wifiSendButton->box.pos = rack::math::Vec(318.f, 38.57f);
		wifiSendButton->app::ParamWidget::module = module;
		wifiSendButton->app::ParamWidget::paramId = HubMedium::wifiSendButtonIndex;
		wifiSendButton->initParamQuantity();
		wifiSendButton->box.pos = wifiSendButton->box.pos.minus(wifiSendButton->box.size.div(2));
		wifiSendButton->click_callback = [this]() {
			if (wifiUrl.length()) {
				wifiSendPatchFile();
			} else {
				promptWifiUrl();
				if (wifiUrl.length()) {
					wifiSendPatchFile();
				}
			}
		};
		addParam(wifiSendButton);
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
		auto yml =
			PatchFileWriter::createPatchYml(hubModule->id, hubModule->mappings, patchName->text, patchDesc->text);
		PatchFileWriter::writeToFile(patchFileName, yml);
	}

	void wifiSendPatchFile() {
		auto patchFileName = cleanupPatchName(patchName->text);
		patchName->text = rack::system::getStem(patchFileName);

		if (rack::system::getExtension(rack::system::getFilename(patchFileName)) != ".yml") {
			patchFileName += ".yml";
		}

		using PatchFileWriter = VCVPatchFileWriter<HubMedium::NumPots, HubMedium::MaxMapsPerPot, MaxKnobSets>;
		auto yml =
			PatchFileWriter::createPatchYml(hubModule->id, hubModule->mappings, patchName->text, patchDesc->text);
		if (yml.size() > 256 * 1024 && wifiVolume == Volume::Internal) {
			statusText->timeToHide = 240;
			statusText->text = "File too large for Internal: max is 256kB";
			return;
		}
		if (yml.size() > 1024 * 1024) {
			statusText->timeToHide = 240;
			statusText->text = "File too large: max is 1MB";
			return;
		}

		statusText->text = "Sending patch file...";

		std::string vol_string = (size_t)wifiVolume < volumeLabels.size() ? volumeLabels[wifiVolume] : "Card";

		auto encoded = FlatBuffers::encode_file(patchFileName, yml, vol_string);
		auto response = network::requestRaw(rack::network::Method::METHOD_POST, wifiUrl + "/action", encoded);
		//TODO: decode response

		statusText->timeToHide = 240;
		if (response == "Failed") {
			statusText->text = "Failed to send patch file";
		} else if (response.length() == 40) {
			statusText->text = "Sent patch file";
		} else {
			statusText->text = "Sent patch file.";
		}
	}

	void promptWifiUrl() {
		auto addr = osdialog_prompt(
			osdialog_message_level::OSDIALOG_INFO, "Enter the address (e.g. http://192.168.1.22)", wifiUrl.c_str());

		if (addr) {
			wifiUrl = addr;
			//TODO: ensure is just http://\d.\d.\d.\d[/]?
			if (wifiUrl.starts_with("https://")) {
				wifiUrl = "http://" + wifiUrl.substr(8);
			}
			if (!wifiUrl.starts_with("http://")) {
				wifiUrl = "http://" + wifiUrl;
			}
			wifiURLText->text = formatWifiStatus();
			free(addr);
		}
	}

	void appendContextMenu(rack::Menu *menu) override {
		using namespace rack;
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuItem("Set Wi-Fi Expander address", "", [this]() { promptWifiUrl(); }));
		menu->addChild(createIndexSubmenuItem(
			"Wi-Fi sends to:",
			volumeLabels,
			[this]() { return wifiVolume; },
			[this](size_t index) {
				wifiVolume = Volume(index);
				wifiURLText->text = formatWifiStatus();
			}));
		// menu->addChild(createCheckMenuItem(
		// 	"Show Wi-Fi button",
		// 	"",
		// 	[this]() { return wifiSendButton->isVisible(); },
		// 	[this]() {
		// 		if (wifiSendButton->isVisible())
		// 			wifiSendButton->hide();
		// 		else
		// 			wifiSendButton->show();
		// 	}));

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
		return wifiUrl + "\n" + (size_t(wifiVolume) < volumeLabels.size() ? volumeLabels[wifiVolume] : "");
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
};

} // namespace MetaModule

rack::Model *modelHubMedium = rack::createModel<MetaModule::HubMedium, MetaModule::HubMediumWidget>("HubMedium");
