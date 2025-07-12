#include <functional>
#include <string>
#include <string_view>

//
#include "osdialog/osdialog.h"
//
#include "filesystem/async_filebrowser.hh"

void async_open_file(std::string_view initial_path,
					 std::string_view filter_extension_list,
					 std::string_view title,
					 std::function<void(char *path)> &&action) {

	osdialog_filters *filters = nullptr;

	if (filter_extension_list == ".wav, .WAV") {
		filters = osdialog_filters_parse("WAV (.wav):wav,WAV");

	} else if (filter_extension_list.length()) {
		//TODO: filter_extension_list => filters
		filters = osdialog_filters_parse(filter_extension_list.data());
	}

	char *path = osdialog_file(OSDIALOG_OPEN, initial_path.data(), nullptr, filters);
	if (path) {
		action(path);
		//action should do this:
		// free(path);
	}

	if (filters)
		osdialog_filters_free(filters);
}
