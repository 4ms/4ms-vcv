#include "generic_module.hh"
#include "CoreModules/4ms/info/Octave_info.hh"
using namespace MetaModule;

rack::Model* modelOctave = GenericModule<OctaveInfo>::create();
