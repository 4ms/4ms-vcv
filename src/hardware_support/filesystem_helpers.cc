#include <filesystem>
#include <string>
#include <string_view>

namespace MetaModule::Filesystem
{

// true if not a MM path
bool is_local_path(std::string_view path) {
	return !(path.starts_with("sdc:") || path.starts_with("usb:") || path.starts_with("nor:") ||
			 path.starts_with("ram:"));
}

std::string translate_path_to_local(std::string_view path, std::string_view local_path, unsigned num_subdirs) {
	std::string path_{path};

	if (is_local_path(path))
		return path_;

	// Convert windows \ to /
	for (auto &c : path_)
		if (c == '\\')
			c = '/';

	auto p = std::filesystem::path{path};
	auto local = std::filesystem::path(local_path);

	// First subdir
	if ((num_subdirs > 0) && p.has_parent_path()) {
		local = local / p.parent_path().filename();

		// Second subdir
		if ((num_subdirs > 1) && p.parent_path().has_parent_path()) {
			local = local / p.parent_path().parent_path().filename();
		}
	}

	local = local / p.filename();

	return local.string();
}

} // namespace MetaModule::Filesystem
