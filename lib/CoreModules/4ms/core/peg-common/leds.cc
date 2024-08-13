#include "peg_base.hh"
#include "log4096.h"

namespace MetaModule::PEG
{

extern const uint16_t log4096[4096];


uint16_t PEGBase::adjust_hue(uint16_t base, uint16_t adj) {
	uint32_t a = (base * adj) >> (kMaxBrightnessBits - 1);
	if (a > kMaxBrightness)
		return kMaxBrightness;
	else
		return a;
}

void PEGBase::create_color(AdjustedColor *col, uint16_t red, uint16_t green, uint16_t blue) {
	col->ping.r = adjust_hue(red, settings.ping_cal_r);
	col->ping.g = adjust_hue(green, settings.ping_cal_g);
	col->ping.b = adjust_hue(blue, settings.ping_cal_b);

	col->cycle.r = adjust_hue(red, settings.cycle_cal_r);
	col->cycle.g = adjust_hue(green, settings.cycle_cal_g);
	col->cycle.b = adjust_hue(blue, settings.cycle_cal_b);

	col->envA.r = adjust_hue(red, settings.enva_cal_r);
	col->envA.g = adjust_hue(green, settings.enva_cal_g);
	col->envA.b = adjust_hue(blue, settings.enva_cal_b);

	col->envB.r = adjust_hue(red, settings.envb_cal_r);
	col->envB.g = adjust_hue(green, settings.envb_cal_g);
	col->envB.b = adjust_hue(blue, settings.envb_cal_b);
}

void PEGBase::adjust_palette(void) {
	create_color(&palette[c_OFF], 0, 0, 0);
	create_color(&palette[c_GREY50], 1024, 1024, 1024);
	create_color(&palette[c_WHITE], 2048, 2048, 2048);
	create_color(&palette[c_FULLWHITE], 4095, 4095, 4095);
	create_color(&palette[c_RED], 4095, 0, 0);
	create_color(&palette[c_ORANGE], 4095, 2720, 0);
	create_color(&palette[c_YELLOW], 3600, 2400, 0);
	create_color(&palette[c_GREEN], 0, 4095, 0);
	create_color(&palette[c_CYAN], 0, 3600, 3600);
	create_color(&palette[c_BLUE], 0, 0, 4095);
	create_color(&palette[c_DIMBLUE], 0, 0, 600);
	create_color(&palette[c_PURPLE], 3600, 0, 4095);
}

void PEGBase::set_rgb_led(RgbLeds rgb_led_id, Palette color_id) {
	if (rgb_led_id == LED_PING) {
		update_pwm(palette[color_id].ping.r, PWM_PINGBUT_R);
		update_pwm(palette[color_id].ping.b, PWM_PINGBUT_B);
		update_pwm(palette[color_id].ping.g, PWM_PINGBUT_G);
	} else if (rgb_led_id == LED_CYCLE) {
		update_pwm(palette[color_id].cycle.r, PWM_CYCLEBUT_R);
		update_pwm(palette[color_id].cycle.b, PWM_CYCLEBUT_B);
		update_pwm(palette[color_id].cycle.g, PWM_CYCLEBUT_G);
	} else if (rgb_led_id == LED_ENVA) {
		update_pwm(palette[color_id].envA.r, PWM_ENVA_R);
		update_pwm(palette[color_id].envA.b, PWM_ENVA_B);
		update_pwm(palette[color_id].envA.g, PWM_ENVA_G);
	} else if (rgb_led_id == LED_ENVB) {
		update_pwm(palette[color_id].envB.r, PWM_ENVB_R);
		update_pwm(palette[color_id].envB.b, PWM_ENVB_B);
		update_pwm(palette[color_id].envB.g, PWM_ENVB_G);
	} else
		return;
}

void PEGBase::set_led_brightness(uint16_t brightness, PwmOutputs pwm_led_num) {
	update_pwm(brightness, pwm_led_num);
}

void PEGBase::all_lights_off() {
	update_pwm(0, PWM_CYCLEBUT_R);
	update_pwm(0, PWM_CYCLEBUT_G);
	update_pwm(0, PWM_CYCLEBUT_B);

	update_pwm(0, PWM_PINGBUT_R);
	update_pwm(0, PWM_PINGBUT_G);
	update_pwm(0, PWM_PINGBUT_B);

	update_pwm(0, PWM_ENVA_R);
	update_pwm(0, PWM_ENVA_G);
	update_pwm(0, PWM_ENVA_B);

	update_pwm(0, PWM_ENVB_R);
	update_pwm(0, PWM_ENVB_G);
	update_pwm(0, PWM_ENVB_B);

	update_pwm(0, PWM_EOF_LED);
}

}
