#include "generic_module.hh"
#include "CoreModules/4ms/info/Quant_info.hh"
using namespace MetaModule;

rack::Model* modelQuant = GenericModule<QuantInfo>::create();
