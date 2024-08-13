#include "generic_module.hh"
#include "CoreModules/4ms/info/PEG_info.hh"
using namespace MetaModule;

rack::Model* modelPEG = GenericModule<PEGInfo>::create();
