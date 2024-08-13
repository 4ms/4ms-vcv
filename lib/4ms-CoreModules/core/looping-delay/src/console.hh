#pragma once
#include "brain_conf.hh"
#include "brain_pinout.hh"
#include "drivers/uart.hh"

constexpr inline UartConf ConsoleUartConf{
	.base_addr = Brain::ConsoleUartBaseAddr,
	.TXPin = Brain::Pin::ConsoleUartTX,
	.RXPin = Brain::Pin::ConsoleUartRX,
	.mode = UartConf::Mode::TXRX,
	.baud = 115200,
	.wordlen = 8,
	.parity = UartConf::Parity::None,
	.stopbits = UartConf::StopBits::_1,
};

struct Console {
	static inline mdrivlib::Uart<ConsoleUartConf> console;
	static void putchar(char c);
	Console() { console.init(); }
	static void init() { console.init(); }
};
