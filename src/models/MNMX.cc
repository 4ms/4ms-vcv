#include "generic_module.hh"
#include "CoreModules/4ms/info/MNMX_info.hh"
using namespace MetaModule;

rack::Model* modelMNMX = GenericModule<MNMXInfo>::create();
