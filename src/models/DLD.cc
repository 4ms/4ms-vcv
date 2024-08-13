#include "generic_module.hh"
#include "CoreModules/4ms/info/DLD_info.hh"
using namespace MetaModule;

rack::Model* modelDLD = GenericModule<DLDInfo>::create();
