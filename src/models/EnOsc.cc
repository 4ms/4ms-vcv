#include "generic_module.hh"
#include "CoreModules/4ms/info/EnOsc_info.hh"
using namespace MetaModule;

rack::Model* modelEnOsc = GenericModule<EnOscInfo>::create();