#include "generic_module.hh"
#include "CoreModules/4ms/info/PitchShift_info.hh"
using namespace MetaModule;

rack::Model* modelPitchShift = GenericModule<PitchShiftInfo>::create();
