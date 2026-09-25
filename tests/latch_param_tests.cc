#include "doctest.h"
#include "mapping/latch_param.hh"

using namespace MetaModule;

TEST_CASE("toggle_value flips to whichever end is farther away") {
	CHECK(toggle_value(0.f, 0.f, 1.f) == 1.f);
	CHECK(toggle_value(1.f, 0.f, 1.f) == 0.f);
	CHECK(toggle_value(0.3f, 0.f, 1.f) == 1.f);
	CHECK(toggle_value(0.7f, 0.f, 1.f) == 0.f);

	// Reversed range
	CHECK(toggle_value(0.f, 1.f, 0.f) == 1.f);
	CHECK(toggle_value(1.f, 1.f, 0.f) == 0.f);
}

TEST_CASE("step_value on continuous or 2-position params toggles") {
	// num_pos = 0: continuous
	CHECK(step_value(0.f, 0.f, 1.f, 0) == 1.f);
	CHECK(step_value(1.f, 0.f, 1.f, 0) == 0.f);
	CHECK(step_value(0.2f, 0.f, 1.f, 0) == 1.f);

	// num_pos = 2: two-position switch
	CHECK(step_value(0.f, 0.f, 1.f, 2) == 1.f);
	CHECK(step_value(1.f, 0.f, 1.f, 2) == 0.f);
}

TEST_CASE("step_value on 3-position switch steps and wraps") {
	CHECK(step_value(0.f, 0.f, 1.f, 3) == 0.5f);
	CHECK(step_value(0.5f, 0.f, 1.f, 3) == 1.f);
	CHECK(step_value(1.f, 0.f, 1.f, 3) == 0.f);
}

TEST_CASE("step_value on multi-position switch steps and wraps") {
	float val = 0.f;
	for (unsigned i = 1; i < 5; i++) {
		val = step_value(val, 0.f, 1.f, 5);
		CHECK(val == doctest::Approx(i / 4.f));
	}
	val = step_value(val, 0.f, 1.f, 5);
	CHECK(val == 0.f);
}

TEST_CASE("step_value stays within min/max range") {
	// 5 positions: 0, 0.25, 0.5, 0.75, 1. Range 0.25..0.75
	CHECK(step_value(0.25f, 0.25f, 0.75f, 5) == doctest::Approx(0.5f));
	CHECK(step_value(0.5f, 0.25f, 0.75f, 5) == doctest::Approx(0.75f));
	CHECK(step_value(0.75f, 0.25f, 0.75f, 5) == doctest::Approx(0.25f));

	// Out of range goes to min
	CHECK(step_value(0.f, 0.25f, 0.75f, 5) == doctest::Approx(0.25f));
	CHECK(step_value(1.f, 0.25f, 0.75f, 5) == doctest::Approx(0.25f));

	// Range set in 1% increments rounds to nearest position
	CHECK(step_value(0.75f, 0.25f, 0.75f, 7) == doctest::Approx(2 / 6.f)); // 0.25 * 6 = 1.5 -> lo = 2
	CHECK(step_value(0.f, 0.33f, 1.f, 4) == doctest::Approx(1 / 3.f));
}

TEST_CASE("step_value with reversed range steps backwards") {
	CHECK(step_value(1.f, 1.f, 0.f, 3) == 0.5f);
	CHECK(step_value(0.5f, 1.f, 0.f, 3) == 0.f);
	CHECK(step_value(0.f, 1.f, 0.f, 3) == 1.f);
}

TEST_CASE("step_value with range spanning less than 2 positions toggles") {
	// 3 positions: 0, 0.5, 1. Range 0.1..0.4 contains no full step
	CHECK(step_value(0.1f, 0.1f, 0.4f, 3) == 0.4f);
	CHECK(step_value(0.4f, 0.1f, 0.4f, 3) == 0.1f);
}

TEST_CASE("default_curve_type is Step for buttons mapped to params with more than 2 positions") {
	CHECK(default_curve_type(true, 3) == 2);
	CHECK(default_curve_type(true, 8) == 2);

	CHECK(default_curve_type(true, 0) == 0);
	CHECK(default_curve_type(true, 2) == 0);

	// Knobs are always Normal
	CHECK(default_curve_type(false, 0) == 0);
	CHECK(default_curve_type(false, 3) == 0);
}
