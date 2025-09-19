#pragma once
#include "CoreModules/hub/MMButtonExpander_info.hh"
#include "hub/hub_module.hh"
#include "hub/hub_module_widget.hh"
#include "patch/mapping_ids.hh"

namespace MetaModule
{

struct ButtonExpanderModule : MetaModuleHubBase {
	ButtonExpanderModule();

	void process(const ProcessArgs &args) override;
	json_t *dataToJson() override;
	void dataFromJson(json_t *) override;

	// Expander Id (0 - 3)
	unsigned buttonExpanderId = 0;
};

struct IndexLabel : rack::Label {
	IndexLabel(ButtonExpanderModule *);
	void onButton(const ButtonEvent &e) override;

private:
	ButtonExpanderModule *parent;
};

struct ButtonExpanderWidget : MetaModuleHubWidget {
	ButtonExpanderWidget(ButtonExpanderModule *module);

	void step() override;
	void appendContextMenu(rack::Menu *menu) override;

private:
	IndexLabel *id_label = nullptr;
	ButtonExpanderModule *buttonExpModule = nullptr;
};

} // namespace MetaModule
