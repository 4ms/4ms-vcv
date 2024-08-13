#include "generic_module.hh"
#include "CoreModules/4ms/info/Tapo_info.hh"
using namespace MetaModule;

rack::Model* modelTapo = GenericModule<TapoInfo>::create();
