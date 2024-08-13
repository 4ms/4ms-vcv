#include "console.hh"

void Console::putchar(char c) { console.putchar(c); }
extern "C" void _putchar(char c) { Console::putchar(c); }
