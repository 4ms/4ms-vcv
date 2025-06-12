#include <functional>
#include <osdialog.h>
#include <string_view>

void async_open_file(std::string_view initial_path,
					 std::string_view filter_extension_list,
					 std::string_view title,
					 std::function<void(char *path)> &&action) {

	osdialog_filters *filters = nullptr;

	if (filter_extension_list == ".wav, .WAV") {
		filters = osdialog_filters_parse("WAV (.wav):wav,WAV");
	}

	char *path = osdialog_file(OSDIALOG_OPEN, initial_path.data(), nullptr, filters);
	if (path) {
		action(path);
		free(path);
	}

	if (filters)
		osdialog_filters_free(filters);
}
