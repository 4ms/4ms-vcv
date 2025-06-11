#include "generic_module.hh"
#include "CoreModules/4ms/info/LPG_info.hh"
using namespace MetaModule;

rack::Model* modelLPG = GenericModule<LPGInfo>::create();
