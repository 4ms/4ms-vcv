#pragma once
#include "brain_pinout.hh"

namespace Debug
{
struct Disabled {
	static void high() {}
	static void low() {}
};

// using Pin0 = Disabled;
using Pin0 = mdrivlib::FPin<Brain::Pin::Debug1.gpio, Brain::Pin::Debug1.pin>;
using Pin1 = mdrivlib::FPin<Brain::Pin::Debug2.gpio, Brain::Pin::Debug2.pin>;
using Pin2 = mdrivlib::FPin<Brain::Pin::Debug3.gpio, Brain::Pin::Debug3.pin>;
using Pin3 = Disabled;

}; // namespace Debug

#define GCC_OPTIMIZE_OFF __attribute__((optimize("-O0")))
