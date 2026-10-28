#pragma once
#include <algorithm>
#include <array>
#include <cctype>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace MetaModule
{

// VCV widgets have no names, so a module's displays are named after their C++ class:
// "Valley::AmalgamTypeDisplay" -> "Amalgam Type Display".
// Returns "" for Rack's generic widget classes, which say nothing about the display.
inline std::string displayNameFromClass(std::string_view demangled) {
	// Strip template arguments, then namespaces
	demangled = demangled.substr(0, demangled.find('<'));
	if (auto pos = demangled.rfind("::"); pos != std::string_view::npos)
		demangled = demangled.substr(pos + 2);

	constexpr std::array<std::string_view, 13> generic{"Widget",
													   "TransparentWidget",
													   "OpaqueWidget",
													   "FramebufferWidget",
													   "LightWidget",
													   "ModuleLightWidget",
													   "SvgWidget",
													   "LedDisplay",
													   "LedDisplayChoice",
													   "LedDisplaySeparator",
													   "LedDisplayTextField",
													   "Label",
													   "TextField"};
	if (std::ranges::find(generic, demangled) != generic.end())
		return "";

	if (demangled.size() > 6 && demangled.ends_with("Widget"))
		demangled.remove_suffix(6);

	// Split CamelCase into words: "LEDScopeView" -> "LED Scope View"
	std::string name;
	for (auto i = 0u; i < demangled.size(); i++) {
		char c = demangled[i];
		if (c == '_') {
			name += ' ';
			continue;
		}
		if (i > 0 && std::isupper((unsigned char)c)) {
			char prev = demangled[i - 1];
			bool next_lower = i + 1 < demangled.size() && std::islower((unsigned char)demangled[i + 1]);
			if (std::islower((unsigned char)prev) || std::isdigit((unsigned char)prev) ||
				(std::isupper((unsigned char)prev) && next_lower))
				name += ' ';
		}
		name += c;
	}
	return name;
}

// Final menu names for a module's displays, given their class names (in order).
// Repeated names get numbered; non-descriptive ones ("" or "Display") become "Display N".
inline std::vector<std::string> nameDisplays(std::vector<std::string> const &class_names) {
	auto is_generic = [](std::string const &n) {
		return n.empty() || n == "Display";
	};

	std::map<std::string, unsigned> count;
	for (auto const &n : class_names) {
		if (!is_generic(n))
			count[n]++;
	}

	std::vector<std::string> names;
	std::map<std::string, unsigned> seen;
	unsigned generic_num = 0;

	for (auto const &n : class_names) {
		if (is_generic(n))
			names.push_back("Display " + std::to_string(++generic_num));
		else if (count[n] > 1)
			names.push_back(n + " " + std::to_string(++seen[n]));
		else
			names.push_back(n);
	}
	return names;
}

} // namespace MetaModule
