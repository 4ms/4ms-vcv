#pragma once

namespace MetaModule
{

inline auto CVToBool = [](float val) -> bool {
	return val >= 0.5f;
};

}