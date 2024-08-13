#include "generic_module.hh"
#include "CoreModules/4ms/info/InfOsc_info.hh"
using namespace MetaModule;

rack::Model* modelInfOsc = GenericModule<InfOscInfo>::create();
