#include "generic_module.hh"
#include "CoreModules/4ms/info/Sequant_info.hh"
using namespace MetaModule;

rack::Model* modelSequant = GenericModule<SequantInfo>::create();
