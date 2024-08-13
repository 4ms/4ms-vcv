#include "generic_module.hh"
#include "CoreModules/4ms/info/ADEnv_info.hh"
using namespace MetaModule;

rack::Model* modelADEnv = GenericModule<ADEnvInfo>::create();
