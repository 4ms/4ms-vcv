#pragma once

#include "pins.hh"
using namespace MetaModule::PEG::Mocks;

namespace MetaModule::PEG
{

struct DigIO
{
	MockedTrigger PingBut;
	MockedTrigger CycleBut;

	MockedTrigger PingJack;
	MockedTrigger CycleJack;
	MockedTrigger TrigJack;

	OutputPin EOJack;
    OutputPin ClockBusOut;

	OutputPin EOJackSecondary;
	
	// //SWO pin can be debug out
	// using DebugOut = mdrivlib::FPin<GPIO::B, PinNum::_3, PinMode::Output, PinPolarity::Normal>;
};

}