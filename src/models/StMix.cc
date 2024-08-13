#include "generic_module.hh"
#include "CoreModules/4ms/info/StMix_info.hh"
using namespace MetaModule;

rack::Model* modelStMix = GenericModule<StMixInfo>::create();
