#pragma once

#include "easiglib/buffer.hh"
#include "easiglib/numtypes.hh"

namespace EnOsc {


struct Data {
	static const easiglib::Buffer<easiglib::s1_15, 257> sine;
	static const easiglib::Buffer<uint32_t, 1024> exp2_u0_23;
	static const easiglib::Buffer<easiglib::f, 17> normalization_factors;
	static const easiglib::Buffer<easiglib::Buffer<easiglib::f, 257>, 12> cheby;
	static const easiglib::Buffer<easiglib::f, 1025> fold;
	static const easiglib::Buffer<easiglib::Buffer<easiglib::f, 9>, 8> triangles;
	static const easiglib::Buffer<easiglib::f, 16> harm;
};

}
