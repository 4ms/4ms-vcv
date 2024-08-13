#include "generic_module.hh"
#include "CoreModules/4ms/info/Fade_info.hh"
using namespace MetaModule;

rack::Model* modelFade = GenericModule<FadeInfo>::create();
