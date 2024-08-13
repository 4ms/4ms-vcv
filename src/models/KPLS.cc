#include "generic_module.hh"
#include "CoreModules/4ms/info/KPLS_info.hh"
using namespace MetaModule;

rack::Model* modelKPLS = GenericModule<KPLSInfo>::create();
