#include "generic_module.hh"
#include "CoreModules/4ms/info/BuffMult_info.hh"
using namespace MetaModule;

rack::Model* modelBuffMult = GenericModule<BuffMultInfo>::create();
