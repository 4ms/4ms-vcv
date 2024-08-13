#include "generic_module.hh"
#include "CoreModules/4ms/info/Gate32_info.hh"
using namespace MetaModule;

rack::Model* modelGate32 = GenericModule<Gate32Info>::create();
