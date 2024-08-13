#include "leds.h"
#include "peg_base.hh"

namespace MetaModule::PEG
{

// static constexpr uint32_t FIRST_CD_POT = ADC_POT_SCALE;

/*

bool MiniPEG::pot_centered(uint8_t pot_num) {
	uint16_t adc = adc_pot_dma_buffer[pot_num];
	return ((adc_pot_dma_buffer[pot_num] > 1800) && (adc_pot_dma_buffer[pot_num] < 2200));
}

CalRequests MiniPEG::should_enter_calibration_mode(void) {
	if (!DigIO::CycleBut::read())
		return CAL_REQUEST_NONE;

	if (!pot_centered(ADC_POT_SCALE))
		return CAL_REQUEST_NONE;

	if (!pot_centered(ADC_POT_OFFSET))
		return CAL_REQUEST_NONE;

	if (!pot_centered(ADC_POT_SHAPE))
		return CAL_REQUEST_NONE;

	if (adc_pot_dma_buffer[ADC_POT_DIVMULT] < 70)
		return CAL_REQUEST_ALL;

	if (pot_centered(ADC_POT_DIVMULT))
		return CAL_REQUEST_LEDS;

	if (adc_pot_dma_buffer[ADC_POT_DIVMULT] > 4000)
		return CAL_REQUEST_CENTER_DET;

	return CAL_REQUEST_NONE;
}

*/

void PEGBase::check_calibration(void) {
	// if (!sanity_check_calibration()) {
	default_settings();
	// 	if (write_settings() != HAL_OK)
	// 		error_writing_settings();
	// }

	// CalRequests c = should_enter_calibration_mode();
	// if (c != CAL_REQUEST_NONE) {
	// 	if (c == CAL_REQUEST_ALL)
	// 		calibrate_divmult_pot();

	// 	if (c == CAL_REQUEST_ALL || c == CAL_REQUEST_CENTER_DET)
	// 		calibrate_center_detents();

	// 	if (c == CAL_REQUEST_ALL || c == CAL_REQUEST_LEDS)
	// 		calibrate_led_colors();

	// 	if (write_settings() != HAL_OK)
	// 		error_writing_settings();
	// }
}

void PEGBase::default_calibration(void) {
	settings.center_detent_offset[DET_SCALE] = 0;
	settings.center_detent_offset[DET_OFFSET] = 0;
	settings.center_detent_offset[DET_SHAPE] = 0;

	settings.ping_cal_r = 2048;
	settings.ping_cal_g = 2048;
	settings.ping_cal_b = 2048;
	settings.cycle_cal_r = 2048;
	settings.cycle_cal_g = 2048;
	settings.cycle_cal_b = 2048;
	// settings.lock_cal_r = 2048;
	// settings.lock_cal_g = 2048;
	// settings.lock_cal_b = 2048;
	settings.enva_cal_r = 2048;
	settings.enva_cal_g = 2048;
	settings.enva_cal_b = 2048;
	settings.envb_cal_r = 2048;
	settings.envb_cal_g = 2048;
	settings.envb_cal_b = 2048;
}

/* 

uint8_t MiniPEG::sanity_check_calibration(void) {
	uint8_t j;

	for (j = 0; j < NUM_CENTER_DETENT_POTS; j++) {
		if ((settings.center_detent_offset[j] < -1000) || (settings.center_detent_offset[j] > 1000))
			return 0;
	}

	if (settings.limit_skew > 1)
		return 0;
	if (settings.free_running_ping > 1)
		return 0;
	if (settings.trigout_is_trig > 1)
		return 0;
	if (settings.trigin_function >= NUM_TRIGIN_FUNCTIONS)
		return 0;
	if (settings.trigout_function >= NUM_TRIGOUT_FUNCTIONS)
		return 0;
	if (settings.cycle_jack_behavior >= NUM_CYCLEJACK_BEHAVIORS)
		return 0;
	if (settings.start_clk_time == 0)
		return 0;
	if (settings.start_cycle_on > 1)
		return 0;

	if (settings.ping_cal_r < 1 || settings.ping_cal_r > 4096)
		return 0;
	if (settings.ping_cal_b < 1 || settings.ping_cal_b > 4096)
		return 0;
	if (settings.cycle_cal_r < 1 || settings.cycle_cal_r > 4096)
		return 0;
	if (settings.cycle_cal_g < 1 || settings.cycle_cal_g > 4096)
		return 0;

	return 1; //pass
}

void MiniPEG::calibrate_center_detents(void) {
	const uint16_t stab_delay = 15;
	CenterDetentPots cur = DET_SCALE;
	uint16_t read_tot;
	uint16_t read_avg;
	uint16_t read1, read2, read3, read4;
	int16_t t;
	Palette color;

	set_rgb_led(LED_PING, c_OFF);
	set_rgb_led(LED_CYCLE, c_OFF);

	while (DigIO::PingBut::read())
		;
	while (DigIO::CycleBut::read())
		;

	HAL_Delay(100);
	set_rgb_led(LED_PING, c_WHITE);
	set_rgb_led(LED_CYCLE, c_WHITE);
	HAL_Delay(100);
	set_rgb_led(LED_PING, c_OFF);
	set_rgb_led(LED_CYCLE, c_OFF);
	HAL_Delay(100);
	set_rgb_led(LED_PING, c_WHITE);
	set_rgb_led(LED_CYCLE, c_WHITE);
	HAL_Delay(100);
	set_rgb_led(LED_PING, c_OFF);
	set_rgb_led(LED_CYCLE, c_OFF);

	while (DigIO::PingBut::read())
		;
	while (DigIO::CycleBut::read())
		;

	while (cur < NUM_CENTER_DETENT_POTS) {
		HAL_Delay(stab_delay);
		read1 = adc_pot_dma_buffer[FIRST_CD_POT + cur];
		HAL_Delay(stab_delay);
		read2 = adc_pot_dma_buffer[FIRST_CD_POT + cur];
		HAL_Delay(stab_delay);
		read3 = adc_pot_dma_buffer[FIRST_CD_POT + cur];
		HAL_Delay(stab_delay);
		read4 = adc_pot_dma_buffer[FIRST_CD_POT + cur];
		read_tot = read1 + read2 + read3 + read4;
		read_avg = read_tot >> 2;

		t = read_avg + settings.center_detent_offset[cur];
		if (t > 2152 || t < 1957)
			color = c_RED; //red: out of range
		else if (t > 2102 || t < 2007)
			color = c_ORANGE; //orange: warning, close to edge
		else if (t > 2068 || t < 2028)
			color = c_YELLOW; //yellow: ok: more than 20 from center
		else
			color = c_GREEN; //green: within 20 of center

		set_rgb_led(LED_CYCLE, color);

		if (DigIO::CycleBut::read()) {
			settings.center_detent_offset[cur] = 2048 - read_avg;
			wait_for_cyclebut_downup();
		}

		if (DigIO::PingBut::read()) {
			wait_for_pingbut_downup();
			cur = static_cast<CenterDetentPots>(cur + 1);
		}
	}
}

// First, Cycle is red, press it to start
// When Cycle but is green, turn Div/Mult knob up a click
// Then Cycle will turn white, press it.
void MiniPEG::calibrate_divmult_pot(void) {
	const uint16_t stab_delay = 15;
	uint16_t calib_array[NUM_DIVMULTS];
	uint16_t read_tot;
	uint16_t read_avg;
	uint16_t read1, read2, read3, read4;
	uint8_t j;
	uint16_t t;
	uint16_t diff;

	set_rgb_led(LED_PING, c_OFF);

	set_rgb_led(LED_CYCLE, c_RED);
	wait_for_cyclebut_downup();
	set_rgb_led(LED_CYCLE, c_OFF);

	for (j = 0; j < NUM_DIVMULTS; j++) {
		set_rgb_led(LED_CYCLE, c_OFF); //off = reading pot

		HAL_Delay(stab_delay);
		read1 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
		HAL_Delay(stab_delay);
		read2 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
		HAL_Delay(stab_delay);
		read3 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
		HAL_Delay(stab_delay);
		read4 = adc_pot_dma_buffer[ADC_POT_DIVMULT];

		read_tot = read1 + read2 + read3 + read4;
		read_avg = read_tot >> 2;

		calib_array[j] = read_avg;

		if (j < (NUM_DIVMULTS - 1)) {
			set_rgb_led(LED_CYCLE, c_GREEN); //blue = ready for user to change knob

			if (j == 0 || j == (NUM_DIVMULTS - 2))
				diff = 80;
			else
				diff = 160;

			HAL_Delay(20);

			//wait until knob is detected as being moved
			do {
				HAL_Delay(stab_delay);
				read1 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
				HAL_Delay(stab_delay);
				read2 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
				HAL_Delay(stab_delay);
				read3 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
				HAL_Delay(stab_delay);
				read4 = adc_pot_dma_buffer[ADC_POT_DIVMULT];

				read_tot = read1 + read2 + read3 + read4;
				read_avg = read_tot >> 2;
			} while ((read_avg - calib_array[j]) < diff);

			set_rgb_led(LED_CYCLE, c_WHITE); //green = press cycle button

			wait_for_cyclebut_downup();
			set_rgb_led(LED_CYCLE, c_OFF);
		}
	}
}

void MiniPEG::calibrate_led_colors(void) {

	set_rgb_led(LED_CYCLE, c_OFF);

	while (DigIO::PingBut::read())
		;
	HAL_Delay(100);

	while (!DigIO::PingBut::read()) {
		update_pwm(adjust_hue(2048, adc_pot_dma_buffer[ADC_POT_SCALE]), PWM_PINGBUT_R);
		update_pwm(adjust_hue(2048, adc_pot_dma_buffer[ADC_POT_SHAPE]), PWM_PINGBUT_G);
		update_pwm(adjust_hue(2048, adc_pot_dma_buffer[ADC_POT_OFFSET]), PWM_PINGBUT_B);
	}
	settings.ping_cal_r = adc_pot_dma_buffer[ADC_POT_SCALE];
	settings.ping_cal_g = adc_pot_dma_buffer[ADC_POT_SHAPE];
	settings.ping_cal_b = adc_pot_dma_buffer[ADC_POT_OFFSET];

	HAL_Delay(100);
	while (DigIO::PingBut::read())
		;
	HAL_Delay(100);

	uint16_t r;
	uint16_t g;
	uint16_t b;

	while (!DigIO::PingBut::read()) {
		if (!DigIO::CycleBut::read()) {
			r = 4095;
			g = 600;
			b = 0;
		} else {
			r = 2048;
			g = 2048;
			b = 2048;
		}
		update_pwm(adjust_hue(r, adc_pot_dma_buffer[ADC_POT_SCALE]), PWM_CYCLEBUT_R);
		update_pwm(adjust_hue(g, adc_pot_dma_buffer[ADC_POT_SHAPE]), PWM_CYCLEBUT_G);
		update_pwm(adjust_hue(b, adc_pot_dma_buffer[ADC_POT_OFFSET]), PWM_CYCLEBUT_B);
	}
	settings.cycle_cal_r = adc_pot_dma_buffer[ADC_POT_SCALE];
	settings.cycle_cal_g = adc_pot_dma_buffer[ADC_POT_SHAPE];
	settings.cycle_cal_b = adc_pot_dma_buffer[ADC_POT_OFFSET];

	all_lights_off();
}

static void wait_for_pingbut_downup(void) {
	uint32_t t = 0;
	while (t < 100)
		t = DigIO::PingBut::read() ? t + 1 : 0;
	t = 0;
	while (t < 100)
		t = !DigIO::PingBut::read() ? t + 1 : 0;
}

static void wait_for_cyclebut_downup(void) {
	uint32_t t = 0;
	while (t < 100)
		t = DigIO::CycleBut::read() ? t + 1 : 0;
	t = 0;
	while (t < 100)
		t = !DigIO::CycleBut::read() ? t + 1 : 0;
}

void error_writing_settings(void) {
	int loops = 20;
	while (loops--) {
		set_rgb_led(LED_PING, c_WHITE);
		set_rgb_led(LED_CYCLE, c_WHITE);
		HAL_Delay(100);
		set_rgb_led(LED_PING, c_RED);
		set_rgb_led(LED_CYCLE, c_RED);
		HAL_Delay(100);
	}
}
*/

} // namespace MetaModule::PEG
