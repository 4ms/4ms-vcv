#include "generic_module.hh"
#include "CoreModules/4ms/info/Verb_info.hh"
using namespace MetaModule;

rack::Model* modelVerb = GenericModule<VerbInfo>::create();
