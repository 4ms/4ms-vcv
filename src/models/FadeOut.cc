#include "generic_module.hh"
#include "CoreModules/4ms/info/FadeOut_info.hh"
using namespace MetaModule;

rack::Model* modelFadeOut = GenericModule<FadeOutInfo>::create();
