#include <string>
#include <string_view>

// Note: std::filesystem is avoided here because it's not available on macOS < 10.15,
// and VCV Rack supports older macOS versions.

namespace MetaModule::Filesystem
{

namespace
{

std::string_view filename(std::string_view path) {
	auto pos = path.find_last_of('/');
	return pos == std::string_view::npos ? path : path.substr(pos + 1);
}

std::string_view parent_path(std::string_view path) {
	auto pos = path.find_last_of('/');
	if (pos == std::string_view::npos)
		return {};

	auto end = path.find_last_not_of('/', pos);
	// Path is like "/file" or "//file": parent is the root
	if (end == std::string_view::npos)
		return path.substr(0, 1);

	return path.substr(0, end + 1);
}

void append(std::string &base, std::string_view part) {
	if (part.starts_with('/'))
		base.clear();
	else if (!base.empty() && !base.ends_with('/'))
		base += '/';
	base += part;
}

} // namespace

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

	std::string_view p{path_};
	std::string local{local_path};

	// First subdir
	if ((num_subdirs > 0) && !parent_path(p).empty()) {
		append(local, filename(parent_path(p)));

		// Second subdir
		if ((num_subdirs > 1) && !parent_path(parent_path(p)).empty()) {
			append(local, filename(parent_path(parent_path(p))));
		}
	}

	append(local, filename(p));

	return local;
}

} // namespace MetaModule::Filesystem
