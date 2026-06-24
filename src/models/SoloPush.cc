#include "generic_module.hh"
#include "CoreModules/4ms/info/SoloPush_info.hh"
using namespace MetaModule;

rack::Model* modelSoloPush = GenericModule<SoloPushInfo>::create();
