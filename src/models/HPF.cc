#include "generic_module.hh"
#include "CoreModules/4ms/info/HPF_info.hh"
using namespace MetaModule;

rack::Model* modelHPF = GenericModule<HPFInfo>::create();
