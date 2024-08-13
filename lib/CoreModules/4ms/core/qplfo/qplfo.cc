#include "qplfo.hh"

namespace MetaModule::QPLFO
{

/********************
 * Settings			*
 ********************/
#define ADC_DRIFT 50

#define PLUCKY_CURVE_ADC 130
#define PLUCKY_CUBIC_ADC 90
#define PLUCKY_QUADRATIC_ADC 55
#define TRIGOUT_CURVE_ADC 30

#define TRIGOUT_WIDTH 50

#define SKEW_TRACKING_TIME 4000

#define HOLDTIMECLEAR 15000

//#define FREERUN




inline uint8_t diff(uint8_t a, uint8_t b){
	if (a>b) return (a-b);
	else return (b-a);
}



Module::Module()
{
    init_tmrs();

    TAPLED_OFF(1);
	TAPLED_OFF(2);
	TAPLED_OFF(3);
	TAPLED_OFF(0);

    tapled_state[0]=0;
	tapled_state[1]=0;
	tapled_state[2]=0;
	tapled_state[3]=0;

	running[0]=0;
	running[1]=0;
	running[2]=0;
	running[3]=0;

    // set to something reasonably large
    // to avoid div by 0 during initial processing
	clk_time[0]=100;
	clk_time[1]=100;
	clk_time[2]=100;
	clk_time[3]=100;
}

void Module::doTick() {

 	tmr_ping[0]++;
	tmr_ping[1]++;
	tmr_ping[2]++;
	tmr_ping[3]++;
	
    tmr_reset[0]++;
	tmr_reset[1]++;
	tmr_reset[2]++;
	tmr_reset[3]++;
	tmr_tapin[0]++;
	tmr_tapin[1]++;
	tmr_tapin[2]++;
	tmr_tapin[3]++;
	tmr_tapout[0]++;
	tmr_tapout[1]++;
	tmr_tapout[2]++;
	tmr_tapout[3]++;

	timer_overflowed++;
}


void Module::init_tmrs(void)
{
	tmr_ping[0]=0;
	tmr_ping[1]=0;
	tmr_ping[2]=0;
	tmr_ping[3]=0;
	tmr_reset[0]=0;
	tmr_reset[1]=0;
	tmr_reset[2]=0;
	tmr_reset[3]=0;
	tmr_tapin[0]=0;
	tmr_tapin[1]=0;
	tmr_tapin[2]=0;
	tmr_tapin[3]=0;
	tmr_tapout[0]=0;
	tmr_tapout[1]=0;
	tmr_tapout[2]=0;
	tmr_tapout[3]=0;
}


uint32_t Module::calc_rise_time(uint32_t skew_val, uint32_t period)
{
	if (skew_val<=(PLUCKY_CURVE_ADC+ADC_DRIFT+ADC_DRIFT))
	{
		return(1);
	}
	else if (skew_val>4000)
	{
		return(period-1);
	}
	else if ((skew_val>1975) && (skew_val<2125))
	{
		return(period>>1);
	}
	else
	{
		return((skew_val * period) >> 12);
	}
}

void Module::updateNextChannel()
{

    if (++chan>=4) chan=0;

    /***************** READ PING *********************
     *  On rising edge of input ping, record the time
     *  since the last ping. Save this into clk_time
     *************************************************/

    if (TAPBUT(chan))
    {
        if (!(tapin_up[chan]))
        {
            if (running[chan]==1) running[chan]=2;
            else if (running[chan]==0) running[chan]=1;

            tapled_state[chan]=1;
            TAPLED_ON(chan);
            tapin_up[chan]=1;

            clk_time[chan] = tmr_tapin[chan];

            tmr_tapin[chan]=0;
            tmr_tapout[chan]=0;
            tmr_reset[chan]=0;
            ready_to_reset[chan]=1;

            clk_time_changed[chan]=1;
        }
        //Check for long hold of tap button
        if (tmr_tapin[chan] > HOLDTIMECLEAR)
        {
            if (running[chan])
            {
                TAPLED_OFF(chan);
                tapled_state[chan]=0;
            }
            running[chan]=0;
            //clk_time[chan]=0;
        }
    } else {
        if (tapin_up[chan])
        {
            tapin_up[chan]=0;
            tapled_state[chan]=0;
            TAPLED_OFF(chan);
        }
    }


    if (PING(chan))
    {
        if (!(ping_high[chan])){
            running[chan]=2;

            tapled_state[chan]=1;
            TAPLED_ON(chan);
            ping_high[chan]=1;

            new_clk_time = tmr_ping[chan];

            if (new_clk_time > clk_time[chan])	t32=new_clk_time - clk_time[chan];
            else t32=clk_time[chan]-new_clk_time;

            if (t32>10){ //better would be to make it a percent of clk_time
                clk_time[chan] = new_clk_time;
                clk_time_changed[chan]=1;
            }

            tmr_ping[chan]=0;
            tmr_tapout[chan]=0;
            tmr_reset[chan]=0;
            ready_to_reset[chan]=1;

        }
    } else {
        if (ping_high[chan]){
            ping_high[chan]=0;
            tapled_state[chan]=0;
            TAPLED_OFF(chan);
        }
    }


    //
    // Flash the TAP button LED with a 50% duty cycle
    // Reset Tap LED timer if it overflows the channel's period
    //
    if (running[chan]==2){
        now = tmr_tapout[chan];
        if (tapled_state[chan] && (now >= (clk_time[chan] >> 1)))
        {
            tapled_state[chan]=0;
            TAPLED_OFF(chan);
        }

        if (now >= clk_time[chan]){
            t32 = (now - clk_time[chan]);
            tmr_tapout[chan]=t32;
            tmr_reset[chan]=t32;

            //tmr_tapout[chan]=0;

            tapled_state[chan]=1;
            ready_to_reset[chan]=1;
            TAPLED_ON(chan);
        }
    }


    if (clk_time_changed[chan])
    {
        clk_time_changed[chan]=0;

        tmr_reset[chan]=0;

        //clk_time changed, so see if reset_offset_time is still valid
        //reset it to 0 if it's greater than the ping period or 2ms under the period
        if ((reset_offset_time[chan]+20)>=clk_time[chan]) {
            reset_offset_time[chan]=0;
        }

        ready_to_reset[chan]=1;

        rise_time[chan] = calc_rise_time(skew_adc[chan], clk_time[chan]);
        fall_time[chan] = clk_time[chan] - rise_time[chan];

        rise_inc[chan] = (1<<28)/(rise_time[chan]);
        fall_inc[chan] = (1<<28)/(fall_time[chan]);

    }



    if (RESETJACK(chan))
    {
        if (!reset_up[chan])
        {
        //	if (env_state!=WAIT){
                reset_offset_time[chan]=tmr_reset[chan];

                //see if the new reset_offset_time is still valid
                //reset it to 0 if it's greater than the ping period or 2ms under the period
                if ((reset_offset_time[chan]+20)>=clk_time[chan]) {
                    reset_offset_time[chan]=0;
                }
        //	}

            reset_now_flag[chan]=1;
            reset_up[chan]=1;
            ready_to_reset[chan]=0;
        }
        else
        {
            if (env_state[chan]!=WAIT)
                ready_to_reset[chan]=0; //disable resetting if RESET is held high "analog mode"
        }
    }
    else
    {
        reset_up[chan]=0;
    }



    /*******************
    *
    * Reset Lock Point
    *
    ********************/

    t32=tmr_reset[chan];

    if (ready_to_reset[chan] && (t32>reset_offset_time[chan]))
    {
        //If we haven't recently modulated skew, then force a reset
        if (is_modulating_skew[chan]==0)
            reset_now_flag[chan]=1;

        if (env_state[chan]==WAIT)
            reset_now_flag[chan]=1;
        ready_to_reset[chan]=0;
    }



    /********************* READ SKEW ADC ************************
     * 		Read the ADC for the Skew value.       			*
     *		If it's changed more than ADC_DRIFT,    		*
        *		re-cacluate the rise and fall times     		*
        ********************************************************/

    t16=adc_buffer[chan];
    if (skew_adc[chan]>t16)
        t=skew_adc[chan]-t16;
    else
        t=t16-skew_adc[chan];

    if (t>ADC_DRIFT)
    {
        is_modulating_skew[chan]=SKEW_TRACKING_TIME;
        skew_adc[chan]=adc_buffer[chan];

        rise_time[chan] = calc_rise_time(skew_adc[chan], clk_time[chan]);
        fall_time[chan]=clk_time[chan]-rise_time[chan];

        rise_inc[chan]=(1<<28)/(rise_time[chan]);
        fall_inc[chan]=(1<<28)/(fall_time[chan]);

    }
    else
    {
        if (is_modulating_skew[chan]>0)
            is_modulating_skew[chan]--;
    }


    /**************** UPDATE THE ENVELOPE *******************
     *  Update only when timer has overflowed
     * -restart if needed
     * -calculate new position
     * -change curve step (RISE/FALL)
     ********************************************************/

    if (chan==3)
    {

        t_of=timer_overflowed;

        if (t_of /*|| reset_now_flag[i]*/)
        {

            for (i=0;i<4;i++)
            {

                // Handle the reset_now_flag by resetting the envelope
                if (reset_now_flag[i])
                {
                    reset_now_flag[i]=0;

                    env_state[i]=RISE;

                    if (skew_adc[i]<=PLUCKY_CURVE_ADC)
                    {
                        if (skew_adc[i]<=TRIGOUT_CURVE_ADC)
                            do_plucky_curve[i]=1;
                        else
                            do_plucky_curve[i]=skew_adc[i];
                    }
                    else
                    {
                        do_plucky_curve[i]=0;
                    }

                    accum[i]=0;
                    sample_ctr[i]=0;
                }

                t_dacout[i]=0;
                sample_ctr[i]++;
                switch (env_state[i])
                {
                    case(RISE):
                        accum[i] += rise_inc[i] * t_of;
                        t_dacout[i]=accum[i]>>18;

                        if (t_dacout[i]>=1023)
                        {
                            accum[i]=1023<<18;
                            t_dacout[i]=1023;
                            env_state[i]=FALL;
                        }
                    break;


                    case(FALL):

                        if (accum[i] > (fall_inc[i] * t_of))
                        accum[i] -= fall_inc[i] * t_of;
                        else accum[i]=0;

                        t_dacout[i] = accum[i]>>18;

                        if ((t_dacout[i]<1) || (t_dacout[i]>1023)){
                            accum[i]=0;
                            t_dacout[i]=0;

                            #ifndef FREERUN
                                env_state[i]=WAIT;

                                //when modulating skew with a slower LFO:
                                //usually we don't get to env_state=RISE because of the clk_time comparison, but sometimes it's because is_mod_skew==0
                                if (RESETJACK(i) || (is_modulating_skew[i]>0)){

                                    t32=tmr_reset[i]>>1; //half the time since the last ping

                                    if (t32<clk_time[i])//if time since last ping is less than twice the LFO period
                                    {
                                        env_state[i]=RISE;

                                        if (skew_adc[i]<=PLUCKY_CURVE_ADC)
                                        {
                                            if (skew_adc[i]<=TRIGOUT_CURVE_ADC)
                                                do_plucky_curve[i]=1;
                                            else
                                                do_plucky_curve[i] = skew_adc[i];
                                        }
                                        else
                                        {
                                            do_plucky_curve[i]=0;
                                        }
                                        sample_ctr[i]=0;

                                    }
                                }
                            #else
                                reset_now_flag[i]=1;
                            #endif
                            //clk_time_changed[i]=0;
                        }
                    break;

                    default:
                        env_state[i]=WAIT;

                        accum[i]=0;
                        t_dacout[i]=0;
                    break;

                }

                if (do_plucky_curve[i])
                {
                    //TRIGOUT
                    if (do_plucky_curve[i]==1)
                    {
                        if (sample_ctr[i]<TRIGOUT_WIDTH)
                            t_dacout[i]=1023;
                        else
                            t_dacout[i]=1;
                    }
                    else //CURVEOUT
                    {
                        //square
                        t_dacout[i]=(((int32_t)t_dacout[i])*((int32_t)t_dacout[i]))>>10;

                        //cubic
                        if (do_plucky_curve[i]<=PLUCKY_CUBIC_ADC)
                            t_dacout[i]=(((int32_t)t_dacout[i])*((int32_t)t_dacout[i]))>>10;

                        if (do_plucky_curve[i]<=PLUCKY_QUADRATIC_ADC)
                            t_dacout[i]=(((int32_t)t_dacout[i])*((int32_t)t_dacout[i]))>>10;
                    }

                }

            } //loop i

            timer_overflowed-=t_of;

            setDACChannel(0, t_dacout[0]);
            setDACChannel(1, t_dacout[1]);
            setDACChannel(2, t_dacout[2]);
            setDACChannel(3, t_dacout[3]);


        } //if (t_of)
    } //chan==3
}

}