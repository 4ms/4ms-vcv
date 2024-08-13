#include "generic_module.hh"
#include "CoreModules/4ms/info/Prob8_info.hh"
using namespace MetaModule;

rack::Model* modelProb8 = GenericModule<Prob8Info>::create();
