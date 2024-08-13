#include "generic_module.hh"
#include "CoreModules/4ms/info/RCD_info.hh"
using namespace MetaModule;

rack::Model* modelRCD = GenericModule<RCDInfo>::create();
