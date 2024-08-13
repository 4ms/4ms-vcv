#pragma once

#include <cstdint>
#include <array>

/************************
 * Mnuemonics			*
 ************************/
#define WAIT 0
#define RISE 1
#define FALL 2

namespace MetaModule::QPLFO
{

class Module
{
public:
    Module();

    void doTick();
    void updateNextChannel();

private:
    void init_tmrs();

    static uint32_t calc_rise_time(uint32_t skew_val, uint32_t period);

private:
    /************************
     * Time keeping			*
     ************************/
    uint32_t tmr_ping[4]={0,0,0,0};
    uint32_t tmr_reset[4]={0,0,0,0};
    uint32_t tmr_tapin[4]={0,0,0,0};
    uint32_t tmr_tapout[4]={0,0,0,0};
    uint32_t ping_irq_timestamp[4]={0,0,0,0};

    /********************
     * Global variables	*
     ********************/
    uint8_t timer_overflowed=0;
    uint8_t ping_state[4]={0,0,0,0};
    uint8_t clk_time_changed[4]={0,0,0,0};
    uint16_t adc_buffer[4];
    int16_t t_dacout[4]={0,0,0,0};

    // Local variables
    uint32_t clk_time[4]={0,0,0,0};

	uint8_t env_state[4]={RISE, RISE, RISE, RISE};
	uint8_t reset_now_flag[4]={0,0,0,0};

	uint32_t rise_time[4]={0,0,0,0};
	uint32_t fall_time[4]={0,0,0,0};

	uint32_t rise_inc[4]={0,0,0,0};
	uint32_t fall_inc[4]={0,0,0,0};
	uint32_t accum[4]={0,0,0,0};

	uint32_t reset_offset_time[4]={0,0,0,0};
	uint8_t ready_to_reset[4]={0,0,0,0};
	uint8_t reset_up[4]={0,0,0,0};

	uint32_t skew_adc[4]={2047,2047,2047,2047};

	uint8_t do_plucky_curve[4]={0,0,0,0};
	uint32_t sample_ctr[4]={0,0,0,0};
	uint32_t is_modulating_skew[4]={0,0,0,0};

	uint8_t t=0;
	uint8_t i;
	uint32_t t32=0;
	uint16_t t16;
	uint32_t new_clk_time;

	uint8_t t_of;

	uint32_t now;

	uint8_t chan=0;

	uint8_t running[4];
	uint8_t tapin_up[4]={0,0,0,0};
	uint8_t tapled_state[4];
	uint8_t ping_high[4]={0,0,0,0};

private:
    void TAPLED_ON(uint8_t x);
    void TAPLED_OFF(uint8_t x);
    uint32_t TAPBUT(uint8_t x);
    uint32_t PING(uint8_t x);
    uint32_t RESETJACK(uint8_t x);

    void setDACChannel(uint8_t, int16_t);

public:
    // To be modified externally
    std::array<bool,4> tapLEDs;
    std::array<bool,4> tapButtons;
    std::array<bool,4> pingInputs;
    std::array<bool,4> resetInputs;

    std::array<float,4> outputs;

    void setADCChannel(uint8_t, float);
};

}
