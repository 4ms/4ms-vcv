#pragma once
#include <cstdint>
#include <span>
#include <string>

namespace MetaModule
{

// Identifies a patch's modules and their order: FNV-1a (32-bit) over each
// module slug followed by '\n'. The MetaModule firmware computes the same hash
// (patch_file/patch_fingerprint.hh) over the patch it's playing.
inline uint32_t patch_fingerprint(std::span<const std::string> slugs) {
	uint32_t hash = 2166136261u;

	auto add = [&hash](char c) {
		hash ^= static_cast<uint8_t>(c);
		hash *= 16777619u;
	};

	for (auto const &slug : slugs) {
		for (char c : slug)
			add(c);
		add('\n');
	}

	return hash;
}

} // namespace MetaModule
