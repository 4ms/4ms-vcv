#pragma once
#include "CoreModules/hub/MMButtonExpander_info.hh"
#include "hub/hub_module.hh"
#include "hub/hub_module_widget.hh"

namespace MetaModule
{

struct ButtonExpanderModule : MetaModuleHubBase {
	using Info = MMButtonExpanderInfo;
	ButtonExpanderModule();
	void process(const ProcessArgs &args) override;
};

struct ButtonExpanderWidget : MetaModuleHubWidget {
	using Info = ButtonExpanderModule::Info;

	ButtonExpanderWidget(MetaModuleHubBase *module);
};

} // namespace MetaModule
