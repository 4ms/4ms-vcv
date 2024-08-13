#include "generic_module.hh"
#include "CoreModules/4ms/info/SHEV_info.hh"
using namespace MetaModule;

rack::Model* modelSHEV = GenericModule<SHEVInfo>::create();
