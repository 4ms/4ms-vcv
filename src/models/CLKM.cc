#include "generic_module.hh"
#include "CoreModules/4ms/info/CLKM_info.hh"
using namespace MetaModule;

rack::Model* modelCLKM = GenericModule<CLKMInfo>::create();
