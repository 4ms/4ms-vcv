#include "generic_module.hh"
#include "CoreModules/4ms/info/FM_info.hh"
using namespace MetaModule;

rack::Model* modelFM = GenericModule<FMInfo>::create();
