#pragma once
#include "rack.hpp"

struct WidgetContext_t {
	rack::ModuleWidget *module_widget;
	rack::Module *module;
};

struct ModuleContext_t {
	rack::Module *module;
};
