#include "generic_module.hh"
#include "CoreModules/4ms/info/Atvert2_info.hh"
using namespace MetaModule;

rack::Model* modelAtvert2 = GenericModule<Atvert2Info>::create();
