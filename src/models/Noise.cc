#include "generic_module.hh"
#include "CoreModules/4ms/info/Noise_info.hh"
using namespace MetaModule;

rack::Model* modelNoise = GenericModule<NoiseInfo>::create();
