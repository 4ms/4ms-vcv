#pragma once

#include <algorithm>
#include <string>

namespace MetaModule
{

inline std::string normalizeRangeValueExpression(std::string text) {
	text.erase(std::remove(text.begin(), text.end(), '%'), text.end());
	return text;
}

} // namespace MetaModule
