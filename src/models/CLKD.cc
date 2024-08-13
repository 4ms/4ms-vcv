#include "generic_module.hh"
#include "CoreModules/4ms/info/CLKD_info.hh"
using namespace MetaModule;

rack::Model* modelCLKD = GenericModule<CLKDInfo>::create();
