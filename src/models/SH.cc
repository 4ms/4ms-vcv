#include "generic_module.hh"
#include "CoreModules/4ms/info/SH_info.hh"
using namespace MetaModule;

rack::Model* modelSH = GenericModule<SHInfo>::create();
