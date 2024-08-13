#include "generic_module.hh"
#include "CoreModules/4ms/info/MultiLFO_info.hh"
using namespace MetaModule;

rack::Model* modelMultiLFO = GenericModule<MultiLFOInfo>::create();
