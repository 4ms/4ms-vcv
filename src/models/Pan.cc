#include "generic_module.hh"
#include "CoreModules/4ms/info/Pan_info.hh"
using namespace MetaModule;

rack::Model* modelPan = GenericModule<PanInfo>::create();
