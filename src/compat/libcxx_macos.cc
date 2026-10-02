// The libc++ that ships with macOS < 10.13 doesn't have std::bad_optional_access or
// std::bad_variant_access. They are thrown by std::optional::value() and std::visit(),
// so the plugin would fail to load with a missing symbol error.
// Defining the key functions here makes the compiler emit the vtables and typeinfo
// into the plugin, so nothing is needed from the system libc++.

#if defined(__APPLE__) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) &&                                   \
	__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101300

#include <optional>
#include <variant>

// libc++ < 18 marks these classes as unavailable before macOS 10.13, so they can't be used at all
#if defined(_LIBCPP_VERSION) && _LIBCPP_VERSION < 180000
#error "Targeting macOS < 10.13 requires libc++ 18 or later (Xcode 16 or later)"
#endif

std::bad_optional_access::~bad_optional_access() noexcept = default;

const char *std::bad_optional_access::what() const noexcept {
	return "bad_optional_access";
}

const char *std::bad_variant_access::what() const noexcept {
	return "bad_variant_access";
}

#endif
