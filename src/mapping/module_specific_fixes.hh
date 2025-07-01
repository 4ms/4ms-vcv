#include "mapping/JackMap.hh"
#include <rack.hpp>

namespace MetaModule
{

// Returns true if the cable was handled already
// Returns false if the caller should handle the cable
bool apply_module_specific_fixes(rack::engine::Cable *cable, std::vector<CableMap> &cableData);

} // namespace MetaModule
