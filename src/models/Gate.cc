#include "generic_module.hh"
#include "CoreModules/4ms/info/Gate_info.hh"
using namespace MetaModule;

rack::Model* modelGate = GenericModule<GateInfo>::create();
