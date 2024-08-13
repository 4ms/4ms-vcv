#include "generic_module.hh"
#include "CoreModules/4ms/info/Switch14_info.hh"
using namespace MetaModule;

rack::Model* modelSwitch14 = GenericModule<Switch14Info>::create();
