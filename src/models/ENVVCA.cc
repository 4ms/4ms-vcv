#include "generic_module.hh"
#include "CoreModules/4ms/info/ENVVCA_info.hh"
using namespace MetaModule;

rack::Model* modelENVVCA = GenericModule<ENVVCAInfo>::create();
