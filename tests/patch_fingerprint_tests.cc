#include "doctest.h"
#include "hub/patch_fingerprint.hh"
#include <string>
#include <vector>

using MetaModule::patch_fingerprint;

// Same vectors as metamodule firmware/tests/patch_fingerprint_tests.cc: both sides must agree
TEST_CASE("patch fingerprint") {
	CHECK(patch_fingerprint(std::vector<std::string>{}) == 2166136261u);
	CHECK(patch_fingerprint(std::vector<std::string>{"HubMedium"}) == 89770027u);
	CHECK(patch_fingerprint(std::vector<std::string>{"HubMedium", "4ms:EnOsc"}) == 1445992889u);
	CHECK(patch_fingerprint(std::vector<std::string>{"4ms:EnOsc", "HubMedium"}) == 2785609493u);
	CHECK(patch_fingerprint(std::vector<std::string>{"HubMedium", "Befaco:EvenVCO", "Befaco:EvenVCO"}) == 4103304475u);
}
