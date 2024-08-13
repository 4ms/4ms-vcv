#include "generic_module.hh"
#include "CoreModules/4ms/info/LPF_info.hh"
using namespace MetaModule;

rack::Model* modelLPF = GenericModule<LPFInfo>::create();
