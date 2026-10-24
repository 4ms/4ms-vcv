#pragma once
#include <cstring>
#include <typeinfo>

namespace MetaModule
{

namespace RttiDetail
{
// Layouts of the Itanium C++ ABI's class type_info objects (used by clang and gcc on
// macOS, Linux and MinGW). macOS's <cxxabi.h> doesn't declare them, so mirror them here.
// See https://itanium-cxx-abi.github.io/cxx-abi/abi.html#rtti-layout
struct SiClassTypeInfo : std::type_info {
	std::type_info const *base;
};

struct BaseClassInfo {
	std::type_info const *base;
	long offset_flags;
};

struct VmiClassTypeInfo : std::type_info {
	unsigned flags;
	unsigned base_count;
	BaseClassInfo bases[1];
};

inline bool is(std::type_info const &info, char const *abi_class) {
	return std::strcmp(typeid(info).name(), abi_class) == 0;
}
} // namespace RttiDetail

// True if `type` is `base` or derives from it, comparing classes by name.
//
// dynamic_cast can't be used for this across plugins: a class with no out-of-line virtual
// function (e.g. rack::app::SvgScrew) has a separate copy of its type_info in every plugin,
// and on macOS those copies are compared by address. Names are the same in every copy.
inline bool derivesFrom(std::type_info const &type, std::type_info const &base) {
	using namespace RttiDetail;

	if (std::strcmp(type.name(), base.name()) == 0)
		return true;

	// Single, public, non-virtual base
	if (is(type, "N10__cxxabiv120__si_class_type_infoE"))
		return derivesFrom(*static_cast<SiClassTypeInfo const &>(type).base, base);

	// Any other bases
	if (is(type, "N10__cxxabiv121__vmi_class_type_infoE")) {
		auto const &vmi = static_cast<VmiClassTypeInfo const &>(type);
		for (unsigned i = 0; i < vmi.base_count; i++) {
			if (derivesFrom(*vmi.bases[i].base, base))
				return true;
		}
	}

	return false;
}

} // namespace MetaModule
