#include "CoreModules/4ms/info/MPEG_info.hh"
#include "generic_module.hh"
using namespace MetaModule;

rack::Model *modelMPEG = GenericModule<MPEGInfo>::create();
