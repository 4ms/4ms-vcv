#include "generic_module.hh"
#include "CoreModules/4ms/info/L4_info.hh"
using namespace MetaModule;

rack::Model* modelL4 = GenericModule<L4Info>::create();
