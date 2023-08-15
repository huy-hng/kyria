#pragma once

#include <zmk/rgb_underglow.h>

#define RGB_SET_HUE 15
#define RGB_SET_SAT 16
#define RGB_SET_BRT 17

// struct zmk_led_hsb {
// 	uint16_t h;
// 	uint8_t s;
// 	uint8_t b;
// };

struct rgb_underglow_state {
	struct zmk_led_hsb color;
	uint8_t animation_speed;
	uint8_t current_effect;
	uint16_t animation_step;
	bool on;
};

// int zmk_rgb_underglow_toggle();
// int zmk_rgb_underglow_get_state(bool *state);
// int zmk_rgb_underglow_on();
// int zmk_rgb_underglow_off();
// int zmk_rgb_underglow_cycle_effect(int direction);
// int zmk_rgb_underglow_calc_effect(int direction);
// int zmk_rgb_underglow_select_effect(int effect);
// struct zmk_led_hsb zmk_rgb_underglow_calc_hue(int direction);
// struct zmk_led_hsb zmk_rgb_underglow_calc_sat(int direction);
// struct zmk_led_hsb zmk_rgb_underglow_calc_brt(int direction);
// int zmk_rgb_underglow_change_hue(int direction);
// int zmk_rgb_underglow_change_sat(int direction);
// int zmk_rgb_underglow_change_brt(int direction);
// int zmk_rgb_underglow_change_spd(int direction);
int zmk_rgb_underglow_set_hsb(struct zmk_led_hsb color);
int zmk_rgb_underglow_set_hue(int value);
int zmk_rgb_underglow_set_sat(int value);
int zmk_rgb_underglow_set_brt(int value);
int zmk_rgb_underglow_set_spd(int value);
struct rgb_underglow_state zmk_rgb_underglow_return_state();
