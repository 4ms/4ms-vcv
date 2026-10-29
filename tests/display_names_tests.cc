#include "doctest.h"
#include "hub/display_names.hh"

using namespace MetaModule;

TEST_CASE("displayNameFromClass") {
	CHECK(displayNameFromClass("ScopeDisplay") == "Scope Display");
	CHECK(displayNameFromClass("Valley::AmalgamTypeDisplay") == "Amalgam Type Display");
	CHECK(displayNameFromClass("bogaudio::Analyzer::Display<3>") == "Display");

	// A trailing "Widget" says nothing
	CHECK(displayNameFromClass("WaveformWidget") == "Waveform");
	CHECK(displayNameFromClass("grain_view") == "grain view");

	// Rack's generic widget classes aren't descriptive
	CHECK(displayNameFromClass("rack::widget::TransparentWidget") == "");
	CHECK(displayNameFromClass("rack::app::LedDisplay") == "");
	CHECK(displayNameFromClass("rack::widget::FramebufferWidget") == "");
	CHECK(displayNameFromClass("Widget") == "");
}

TEST_CASE("nameDisplays") {
	SUBCASE("unique descriptive names are kept") {
		auto names = nameDisplays({"Scope Display", "Grain View"});
		CHECK(names == std::vector<std::string>{"Scope Display", "Grain View"});
	}

	SUBCASE("repeated names are numbered") {
		auto names = nameDisplays({"Text", "Scope", "Text", "Text"});
		CHECK(names == std::vector<std::string>{"Text 1", "Scope", "Text 2", "Text 3"});
	}

	SUBCASE("non-descriptive ones are Display N") {
		auto names = nameDisplays({"", "Scope", ""});
		CHECK(names == std::vector<std::string>{"Display 1", "Scope", "Display 2"});
	}

	SUBCASE("a class literally named Display is numbered with the generic ones") {
		auto names = nameDisplays({"Display", ""});
		CHECK(names == std::vector<std::string>{"Display 1", "Display 2"});
	}
}
