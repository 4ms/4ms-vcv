#include "generic_module.hh"
#include "CoreModules/4ms/info/Switch41_info.hh"
using namespace MetaModule;

rack::Model* modelSwitch41 = GenericModule<Switch41Info>::create();
