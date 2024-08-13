#include "generic_module.hh"
#include "CoreModules/4ms/info/Freeverb_info.hh"
using namespace MetaModule;

rack::Model* modelFreeverb = GenericModule<FreeverbInfo>::create();
