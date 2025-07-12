#include "CoreModules/4ms/info/TSP_info.hh"
#include "generic_module.hh"
using namespace MetaModule;

rack::Model *modelTSP = GenericModule<TSPInfo>::create();
