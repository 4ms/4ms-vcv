#include "generic_module.hh"
#include "CoreModules/4ms/info/Seq4_info.hh"
using namespace MetaModule;

rack::Model* modelSeq4 = GenericModule<Seq4Info>::create();
