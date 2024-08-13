#include "generic_module.hh"
#include "CoreModules/4ms/info/BPF_info.hh"
using namespace MetaModule;

rack::Model* modelBPF = GenericModule<BPFInfo>::create();
