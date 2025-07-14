#include <functional>
#include <string>
#include <string_view>

//
#include "osdialog/osdialog.h"
//
#include "filesystem/async_filebrowser.hh"

namespace MetaModule
{
extern std::string last_file_path;
}

void async_open_file(std::string_view initial_path,
					 std::string_view filter_extension_list,
					 std::string_view title,
					 std::function<void(char *path)> &&action) {

	if (MetaModule::last_file_path.length()) {
		auto path = strdup(MetaModule::last_file_path.c_str());
		action(path);
	}
}
