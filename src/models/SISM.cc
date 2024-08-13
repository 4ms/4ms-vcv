#include "generic_module.hh"
#include "CoreModules/4ms/info/SISM_info.hh"
using namespace MetaModule;

rack::Model* modelSISM = GenericModule<SISMInfo>::create();
