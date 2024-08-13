#include "generic_module.hh"
#include "CoreModules/4ms/info/Detune_info.hh"
using namespace MetaModule;

rack::Model* modelDetune = GenericModule<DetuneInfo>::create();
