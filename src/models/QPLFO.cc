#include "generic_module.hh"
#include "CoreModules/4ms/info/QPLFO_info.hh"
using namespace MetaModule;

rack::Model* modelQPLFO = GenericModule<QPLFOInfo>::create();
