#include "generic_module.hh"
#include "CoreModules/4ms/info/Comp_info.hh"
using namespace MetaModule;

rack::Model* modelComp = GenericModule<CompInfo>::create();
