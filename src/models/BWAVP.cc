#include "CoreModules/4ms/info/BWAVP_info.hh"
#include "generic_module.hh"
using namespace MetaModule;

rack::Model *modelBWAVP = GenericModule<BWAVPInfo>::create();
