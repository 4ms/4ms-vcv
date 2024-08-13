#pragma once

#include "easiglib/dsp.hh"

namespace EnOsc
{

static constexpr int sine_size = 512 + 1;
static constexpr int cheby_tables = 16;
static constexpr int cheby_size = 512 + 1;
static constexpr int fold_size = 1024 + 1;

struct DynamicData {
	DynamicData();
	using s1_15 = easiglib::s1_15;
	using f = easiglib::f;

	static easiglib::Buffer<std::pair<s1_15, s1_15>, sine_size> sine;
	static easiglib::Buffer<easiglib::Buffer<f, cheby_size>, cheby_tables> cheby;
	static easiglib::Buffer<std::pair<f, f>, fold_size> fold;
	static easiglib::Buffer<f, (fold_size - 1) / 2 + 1> fold_max;
	static easiglib::Buffer<easiglib::Buffer<f, 9>, 8> triangles;
};

} // namespace EnOsc
