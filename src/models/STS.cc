#include "generic_module.hh"
#include "CoreModules/4ms/info/STS_info.hh"
using namespace MetaModule;

rack::Model* modelSTS = GenericModule<STSInfo>::create();
