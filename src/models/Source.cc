#include "generic_module.hh"
#include "CoreModules/4ms/info/Source_info.hh"
using namespace MetaModule;

rack::Model* modelSource = GenericModule<SourceInfo>::create();
