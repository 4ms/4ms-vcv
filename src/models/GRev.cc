#include "generic_module.hh"
#include "CoreModules/4ms/info/GRev_info.hh"
using namespace MetaModule;

rack::Model* modelGRev = GenericModule<GRevInfo>::create();
