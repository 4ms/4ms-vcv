#include "generic_module.hh"
#include "CoreModules/4ms/info/DEV_info.hh"
using namespace MetaModule;

rack::Model* modelDEV = GenericModule<DEVInfo>::create();
