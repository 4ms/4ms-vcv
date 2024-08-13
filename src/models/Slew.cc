#include "generic_module.hh"
#include "CoreModules/4ms/info/Slew_info.hh"
using namespace MetaModule;

rack::Model* modelSlew = GenericModule<SlewInfo>::create();
