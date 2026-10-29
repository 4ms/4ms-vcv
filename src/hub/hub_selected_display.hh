#pragma once
#include <cstdint>
#include <jansson.h>
#include <optional>

namespace MetaModule
{

// The display chosen in the hub's menu to be the patch's selected display on MetaModule.
// 4ms modules' displays are identified by their light index. Other modules' displays are
// identified by their position, since VCV can't know the light index MetaModule gives them.
struct HubSelectedDisplay {
	int64_t moduleId = -1;
	std::optional<uint16_t> lightId{};
	float x_mm = 0;
	float y_mm = 0;

	bool is_set() const {
		return moduleId >= 0;
	}

	bool operator==(HubSelectedDisplay const &) const = default;

	json_t *encodeJson() const {
		json_t *j = json_object();
		json_object_set_new(j, "ModuleId", json_integer(moduleId));
		if (lightId) {
			json_object_set_new(j, "LightId", json_integer(*lightId));
		} else {
			json_object_set_new(j, "X", json_real(x_mm));
			json_object_set_new(j, "Y", json_real(y_mm));
		}
		return j;
	}

	static HubSelectedDisplay decodeJson(json_t *j) {
		HubSelectedDisplay ss{};

		auto moduleIdJ = json_object_get(j, "ModuleId");
		if (!json_is_integer(moduleIdJ))
			return {};
		ss.moduleId = json_integer_value(moduleIdJ);

		auto lightIdJ = json_object_get(j, "LightId");
		auto xJ = json_object_get(j, "X");
		auto yJ = json_object_get(j, "Y");

		if (json_is_integer(lightIdJ)) {
			ss.lightId = json_integer_value(lightIdJ);
		} else if (json_is_number(xJ) && json_is_number(yJ)) {
			ss.x_mm = json_number_value(xJ);
			ss.y_mm = json_number_value(yJ);
		} else {
			return {};
		}
		return ss;
	}
};

} // namespace MetaModule
