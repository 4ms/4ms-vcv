#include "generic_module.hh"
#include "CoreModules/4ms/info/PI_info.hh"
using namespace MetaModule;

rack::Model* modelPI = GenericModule<PIInfo>::create();
