#include "generic_module.hh"
#include "CoreModules/4ms/info/Seq8_info.hh"
using namespace MetaModule;

rack::Model* modelSeq8 = GenericModule<Seq8Info>::create();
