#include "CoreModules/4ms/info/Mixer4_info.hh"
#include "generic_module.hh"
using namespace MetaModule;

rack::Model *modelMixer = GenericModule<Mixer4Info>::create();
