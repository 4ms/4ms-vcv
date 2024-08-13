#include "generic_module.hh"
#include "CoreModules/4ms/info/VCAM_info.hh"
using namespace MetaModule;

rack::Model* modelVCAM = GenericModule<VCAMInfo>::create();
