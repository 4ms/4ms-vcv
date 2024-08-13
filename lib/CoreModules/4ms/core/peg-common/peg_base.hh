#pragma once

#include <cstdint>

#include "pingable_env.h"
#include "flash_user.hh"
#include "analog_conditioning.h"
#include "mocks/dig_inout_pins.hh"
#include "debounced_digins.h"
#include "leds.h"
#include "envelope_calcs.h"
#include "dac.h"
#include "calibration.hh"

namespace MetaModule::PEG
{

struct PEGBase
{
public:
    PEGBase(EnvelopeCalcsBase *env_calcs);

    void update();

    void pingEdgeIn();

private:
    void read_ping_button();
    void read_trigjacks();
    void read_cycle_button();
    void check_reset_envelopes();
    void update_tap_clock();
    void update_trigout();
    void read_ping_clock();
    void ping_led_off();
    void ping_led_on();
    void handle_qnt_trig(PingableEnvelope*);
    void handle_async_trig(PingableEnvelope*);

    PingableEnvelope m;

    uint32_t ping_irq_timestamp = 0;
    uint32_t trig_irq_timestamp = 0;

    uint32_t last_tapin_time = 0;
    uint32_t tapout_clk_time = 0;

    uint32_t clk_time = 0;

    uint8_t cycle_but_on = 0;

    uint8_t using_tap_clock = 0;
    uint8_t do_toggle_cycle = 0;

    uint8_t force_params_update = 0;

    char div_ping_led = 0;
    char got_tap_clock = 0;

    uint32_t entering_system_mode = 0;
    uint8_t system_mode_cur = 0;
    uint8_t initial_cycle_button_state = 0;
    char update_cycle_button_now = 0;

    bool adjusting_shift_mode = false;
    bool toggled_sync_mode = false;
    int16_t cycle_latched_offset = 0;

    analog_t analog[NUM_ADCS];

    void eor_on();
    void eor_off();
    void eof_on();
    void eof_off();
    void hr_on();
    void hr_off();
    void tapclkout_off();
    void tapclkout_on();
    void handle_trigout_trigfall();
    void handle_trigout_secondary_trigfall();
    void clockbus_on();
    void clockbus_off();

    void trigout_on();
    void trigout_off();

    void trigout_secondary_on();
    void trigout_secondary_off();

    uint8_t trigout_high = 0;
    uint8_t trigout_secondary_high = 0;

public:
    // for sideloading
    DigIO digio;

private:
    void condition_analog(void);
    void init_analog_conditioning(void);
    void setup_fir_lpf(void);

    uint8_t oversample_ctr = 0;

public:
    // for sideloading
    uint16_t adc_dma_buffer[NUM_ADCS] = {0};
    uint16_t *adc_cv_dma_buffer = &(adc_dma_buffer[0]);
    uint16_t *adc_pot_dma_buffer = &(adc_dma_buffer[NUM_CV_ADCS]);

private:
    void init_pwm();
    void update_pwm(uint32_t pwmval, enum PwmOutputs channel);

public:
    // for sideloading
    uint32_t pwm_vals[NUM_PWMS] = {0};

private:
    void dac_out(enum DACs dac, uint16_t val);

public:
    // for sideloading
    uint16_t dac_vals[2] = {0};

    // call this with dac sample rate
    static const uint32_t kDacSampleRate = 40000;
    void update_all_envelopes();

private:
    void update_envelope(PingableEnvelope *e);
    void stop_envelope(struct PingableEnvelope *e);
    void start_envelope(struct PingableEnvelope *e);
    void check_restart_async_env(struct PingableEnvelope *e);
    void sync_env_to_clk(struct PingableEnvelope *e);
    uint8_t resync_on_ping(struct PingableEnvelope *e);

    void do_reset_envelope(PingableEnvelope *e);
    void output_env_val(uint16_t rawA);
    void handle_env_segment_end(PingableEnvelope *e, envelopeStates end_segment_flag);
    void handle_env_end(PingableEnvelope *e, uint8_t end_env_flag);
    void start_envelope_in_sync(PingableEnvelope *e);
    void start_envelope_immediate(PingableEnvelope *e);
    int32_t scale_shift_offset_env(uint16_t raw_env_val);

    bool system_mode_active = false;
    int32_t scale = 0;
    int32_t offset = 0;
    int32_t shift = 0; // not used anymore

    int8_t get_clk_div_nominal(uint16_t adc_val);
    uint32_t get_clk_div_time(int8_t clock_divide_amount, uint32_t clk_time);
    uint32_t get_fall_time(uint8_t skew, uint32_t div_clk_time);
    int16_t calc_curve(int16_t t_dacout, uint8_t cur_curve);
	void calc_skew_and_curves(uint16_t skewadc, uint16_t shapeadc, uint8_t *skew, uint8_t *next_curve_rise, uint8_t *next_curve_fall);
    void calc_rise_fall_incs(struct PingableEnvelope *e);
    void calc_div_clk_time(struct PingableEnvelope *e, uint32_t new_clk_time);

    int write_settings(void);
    uint8_t read_settings(void);
    uint8_t check_settings_valid(void);
    void default_settings(void);

public:
	void apply_settings();
	void set_sync_mode(bool);

    // for sideloading
    struct SystemSettings settings;

private:
    void update_adc_params(uint8_t force_params_update);
    void init_params(void);

    int8_t read_divmult();
    uint8_t read_shape_scale_offset();
    void update_clock_divider_amount(struct PingableEnvelope *e, int16_t new_clock_divider_amount);
    void update_env_tracking(struct PingableEnvelope *e);

	EnvelopeCalcsBase *envelope_calcs;
	uint16_t skew = 0;  //ignored by MiniPEG
    uint16_t shape = 0;
    uint16_t oversample_wait_ctr = 0;
	uint16_t poll_user_input = 0;
    int16_t last_total_adc = 0;
	int8_t last_clock_divider_amount = 0;

    void init_pingable_env(struct PingableEnvelope *e);
    void copy_rise_fall_incs(struct PingableEnvelope *dst, struct PingableEnvelope *src);
    void copy_skew_and_curves(struct PingableEnvelope *dst, struct PingableEnvelope *src);

    void init_dig_inouts(void);

    void inc_tmrs(void);

    uint32_t systmr               = 0;
    uint32_t tapouttmr            = 0;
    uint32_t tapintmr             = 0;
    uint32_t pingtmr              = 0;
    uint32_t trigouttmr           = 0;
    uint32_t trigouttmr_secondary = 0;

    bool just_released(DebouncedDiginList pin);
    bool just_pressed(DebouncedDiginList pin);
    bool is_pressed(DebouncedDiginList pin);
    void init_debouncer(void);

    void reset_transition_counter();
    void force_transition();
    bool check_to_start_transition();

    void do_start_transition(struct PingableEnvelope *e);
    void start_transition(struct PingableEnvelope *e, uint32_t elapsed_time);
    int8_t calc_divided_ping_div_ctr(PingableEnvelope *e, envelopeStates envstate);

    uint32_t didnt_change_divmult = 0;

    void check_calibration(void);
    void default_calibration(void);

    // uint8_t sanity_check_calibration(void);
    // void calibrate_divmult_pot(void);
    // CalRequests should_enter_calibration_mode(void);
    // void calibrate_center_detents(void);
    // void calibrate_led_colors(void);
    // void wait_for_pingbut_downup(void);
    // void wait_for_cyclebut_downup(void);
    // void error_writing_settings(void);

    void set_led_brightness(uint16_t brightness, PwmOutputs pwm_led_num);
    void set_rgb_led(RgbLeds rgb_led_id, Palette color_id);
    void adjust_palette(void);
    uint16_t adjust_hue(uint16_t base, uint16_t adj);
    void all_lights_off(void);

    void create_color(AdjustedColor *col, uint16_t red, uint16_t green, uint16_t blue);

    AdjustedColor palette[NUM_COLORS];

    static constexpr uint32_t kMaxBrightness = 4095;
    static constexpr uint32_t kMaxBrightnessBits = 12;

};

}
