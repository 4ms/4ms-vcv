#include "generic_module.hh"
#include "CoreModules/4ms/info/SCM_info.hh"
using namespace MetaModule;

rack::Model* modelSCM = GenericModule<SCMInfo>::create();
