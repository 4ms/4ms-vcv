#include "network.hh"
#include "request_gate.hh"
#include <span>
#include <string>
#include <vector>

#include <openssl/crypto.h>
#define CURL_STATICLIB
#include <curl/curl.h>

#include <asset.hpp>
#include <network.hpp>
#include <settings.hpp>
#include <system.hpp>

namespace MetaModule::network
{

static const std::vector<std::string> methodNames = {
	"GET",
	"POST",
	"PUT",
	"DELETE",
};

static CURL *createCurl() {
	CURL *curl = curl_easy_init();
	assert(curl);

	// curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	std::string userAgent = rack::APP_NAME + " " + rack::APP_EDITION_NAME + "/" + rack::APP_VERSION;
	curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
	// Timeout to wait on initial HTTP connection.
	// This is lower than the typical HTTP timeout of 60 seconds to avoid DAWs from aborting plugin scans.
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);

	// If curl can't resolve a DNS entry, it sends a signal to interrupt the process.
	// However, since we use curl on non-main thread, this crashes the application.
	// So tell curl not to signal.
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

	// Load root certificates
	std::string caPath = rack::asset::system("cacert.pem");
	curl_easy_setopt(curl, CURLOPT_CAINFO, caPath.c_str());

	// Don't verify HTTPS certificates if verifyHttpsCerts is false
	// curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, rack::settings::verifyHttpsCerts);
	// Not using https:
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

	return curl;
}

static size_t writeVectorCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	auto *str = (std::vector<uint8_t> *)userdata;
	size_t len = size * nmemb;
	// A FlatBuffers response may arrive in several HTTP chunks.
	str->insert(str->end(), ptr, ptr + len);
	return len;
}

static std::string getCookieString(const rack::network::CookieMap &cookies) {
	std::string s;
	for (const auto &pair : cookies) {
		s += rack::network::encodeUrl(pair.first);
		s += "=";
		s += rack::network::encodeUrl(pair.second);
		s += ";";
	}
	return s;
}

// The MetaModule handles one Wi-Fi request at a time
static RequestGate request_gate;

// Aborts a background request as soon as a foreground request is waiting
static int abortIfForegroundWaiting(void *, curl_off_t, curl_off_t, curl_off_t, curl_off_t) {
	return request_gate.foreground_waiting() ? 1 : 0;
}

static std::vector<uint8_t> perform(rack::network::Method method,
									const std::string &url,
									const std::span<uint8_t> &reqStr,
									const rack::network::CookieMap &cookies,
									const RequestOptions &options,
									bool background,
									bool &aborted) {
	using namespace rack::network;

	std::string urlS = url;
	CURL *curl = createCurl();

	curl_easy_setopt(curl, CURLOPT_URL, urlS.c_str());

	// Set HTTP method
	if (method == METHOD_GET) {
		// This is CURL's default
	} else if (method == METHOD_POST) {
		curl_easy_setopt(curl, CURLOPT_POST, true);
	} else if (method == METHOD_PUT) {
		curl_easy_setopt(curl, CURLOPT_PUT, true);
	} else if (method == METHOD_DELETE) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	}

	// Set headers
	struct curl_slist *headers = NULL;
	auto timeout_header = "bridge-timeout-ms: " + std::to_string(options.bridge_timeout_ms);
	headers = curl_slist_append(headers, timeout_header.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	// Cookies
	if (!cookies.empty()) {
		curl_easy_setopt(curl, CURLOPT_COOKIE, getCookieString(cookies).c_str());
	}

	// Body callbacks
	if (reqStr.size()) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, reqStr.data());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, reqStr.size());
	}

	std::vector<uint8_t> resBytes;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeVectorCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resBytes);

	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	if (options.total_timeout_ms > 0)
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, options.total_timeout_ms);

	if (background) {
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, abortIfForegroundWaiting);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	}

	// Perform request
	if (options.log)
		INFO("Requesting Raw %s %s", methodNames[method].c_str(), urlS.c_str());
	CURLcode res = curl_easy_perform(curl);

	// Cleanup
	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);

	aborted = (res == CURLE_ABORTED_BY_CALLBACK);

	if (res != CURLE_OK) {
		if (options.log)
			WARN("Could not request %s: %s", urlS.c_str(), curl_easy_strerror(res));
		return {};
	}

	return resBytes;
}

std::vector<uint8_t> requestRaw(rack::network::Method method,
								const std::string &url,
								const std::span<uint8_t> &reqStr,
								const rack::network::CookieMap &cookies,
								const RequestOptions &options) {
	auto lock = request_gate.foreground();
	bool aborted = false;
	return perform(method, url, reqStr, cookies, options, false, aborted);
}

std::optional<std::vector<uint8_t>> tryRequestRawInBackground(rack::network::Method method,
															   const std::string &url,
															   const std::span<uint8_t> &reqStr,
															   const RequestOptions &options) {
	auto lock = request_gate.try_background();
	if (!lock)
		return std::nullopt;

	bool aborted = false;
	auto reply = perform(method, url, reqStr, {}, options, true, aborted);
	if (aborted)
		return std::nullopt;

	return reply;
}

} // namespace MetaModule::network
