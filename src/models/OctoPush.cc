#include "CoreModules/4ms/info/Octopush_info.hh"
#include "generic_module.hh"
using namespace MetaModule;

rack::Model *modelOctoPush = GenericModule<OctoPushInfo>::create();
