#pragma once
#include "CoreModules/elements/element_counter.hh"
#include "config_element_impl.hh"
#include "console/pr_dbg.hh"
#include "vcv_creation_context.hh"

namespace MetaModule
{

inline void do_config_element(BaseElement el, const ElementCount::Indices &, const ModuleContext_t &) {
	// Do nothing by default
	// FIXME: This should probably be replaced with more specific fallbacks
	pr_dbg("Element not configured (%.*s)\n", (int)el.short_name.size(), el.short_name.data());
};

template<typename INFO>
struct ConfigElement {

	ConfigElement(rack::Module *module)
		: context{module} {
	}

	template<typename T>
	void config_element(const T &element) {
		// forward to implementation together with current context
		if (auto indices = ElementCount::get_indices<INFO>(element)) {
			do_config_element(element, indices.value(), context);
		}
	}

private:
	ModuleContext_t context;
};

} // namespace MetaModule
