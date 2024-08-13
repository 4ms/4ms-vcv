#include "generic_module.hh"
#include "CoreModules/4ms/info/Djembe_info.hh"
using namespace MetaModule;

rack::Model* modelDjembe = GenericModule<DjembeInfo>::create();
