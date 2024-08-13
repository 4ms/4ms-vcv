#include "generic_module.hh"
#include "CoreModules/4ms/info/QCD_info.hh"
using namespace MetaModule;

rack::Model* modelQCD = GenericModule<QCDInfo>::create();
