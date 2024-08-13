#include "generic_module.hh"
#include "CoreModules/4ms/info/Drum_info.hh"
using namespace MetaModule;

rack::Model* modelDrum = GenericModule<DrumInfo>::create();
