#include "generic_module.hh"
#include "CoreModules/4ms/info/Gate8_info.hh"
using namespace MetaModule;

rack::Model* modelGate8 = GenericModule<Gate8Info>::create();
