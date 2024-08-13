#include "generic_module.hh"
#include "CoreModules/4ms/info/ComplexEG_info.hh"
using namespace MetaModule;

rack::Model* modelComplexEG = GenericModule<ComplexEGInfo>::create();
