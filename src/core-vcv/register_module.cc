#include "CoreModules/moduleFactory.hh"
#include <string_view>

namespace MetaModule
{

bool register_module(std::string_view brand_name,
					 std::string_view typeslug,
					 ModuleFactory::CreateModuleFunc funcCreate,
					 ModuleInfoView const &info,
					 std::string_view faceplate_filename) {

	return ModuleFactory::registerModuleType(brand_name, typeslug, funcCreate, info, faceplate_filename);
}

} // namespace MetaModule
