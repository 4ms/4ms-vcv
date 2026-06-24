#include "generic_module.hh"
#include "CoreModules/4ms/info/FadeIn_info.hh"
using namespace MetaModule;

rack::Model* modelFadeIn = GenericModule<FadeInInfo>::create();
