#include "qplfo.hh"

namespace MetaModule::QPLFO
{

void Module::TAPLED_ON(uint8_t x)
{
    tapLEDs[x] = true;
}

void Module::TAPLED_OFF(uint8_t x)
{
    tapLEDs[x] = false;
}

uint32_t Module::TAPBUT(uint8_t x)
{
    return tapButtons[x];
}

uint32_t Module::PING(uint8_t x)
{
    return pingInputs[x];
}

uint32_t Module::RESETJACK(uint8_t x)
{
    return resetInputs[x];
}

void Module::setDACChannel(uint8_t channel, int16_t value)
{
    // input is 10 bit
    outputs[channel] = float(value) / ((1<<10) - 1);
}

void Module::setADCChannel(uint8_t channel, float value)
{
    // 12 bit ADC
    adc_buffer[channel] = value * ((1<<12) - 1);
}



}