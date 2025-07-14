#pragma once
#include "comm/comm_module.hh"
#include "rack.hpp"

struct WidgetContext_t {
	rack::ModuleWidget *module_widget;
	CommModule *module;
};

struct ModuleContext_t {
	rack::Module *module;
};
