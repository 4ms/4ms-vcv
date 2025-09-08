#pragma once

#include "jansson.h"
#include <string>
#include <vector>

namespace MetaModule
{

struct JackAlias {
	std::vector<std::string> in{};
	std::vector<std::string> out{};

	json_t *encodeJson() {
		auto rootJ = json_object();

		json_t *jackAliasesIn = json_array();
		for (auto &i : in) {
			auto a = json_string(i.c_str());
			json_array_append(jackAliasesIn, a);
		}

		json_object_set_new(rootJ, "Input", jackAliasesIn);

		json_t *jackAliasesOut = json_array();
		for (auto &o : out) {
			auto a = json_string(o.c_str());
			json_array_append(jackAliasesOut, a);
		}

		json_object_set_new(rootJ, "Output", jackAliasesOut);

		return rootJ;
	}

	void decodeJson(json_t *rootJ) {
		auto aliasInJ = json_object_get(rootJ, "Input");
		if (json_is_array(aliasInJ)) {
			for (auto i = 0u; i < json_array_size(aliasInJ); ++i) {
				const auto nameJ = json_array_get(aliasInJ, i);
				const auto t = json_is_string(nameJ) ? json_string_value(nameJ) : "";
				in[i] = std::move(t);
			}
		}

		auto aliasOutJ = json_object_get(rootJ, "Output");
		if (json_is_array(aliasOutJ)) {
			for (auto i = 0u; i < json_array_size(aliasOutJ); ++i) {
				const auto nameJ = json_array_get(aliasOutJ, i);
				const auto t = json_is_string(nameJ) ? json_string_value(nameJ) : "";
				out[i] = std::move(t);
			}
		}
	}
};

} // namespace MetaModule
