#include "generic_module.hh"
#include "CoreModules/4ms/info/Follow_info.hh"
using namespace MetaModule;

rack::Model* modelFollow = GenericModule<FollowInfo>::create();
