#pragma once

#include <zephyr/init.h>
#include <zmk/rgb_underglow.h>

#define RGB_SET_HUE 15
#define RGB_SET_SAT 16
#define RGB_SET_BRT 17

#define ANIMATION_REFRESH 25
#define ANIMATION_DURATION 200

struct rgb_underglow_state_extra {
	struct zmk_led_hsb color;
	uint8_t animation_speed;
	uint8_t current_effect;
	uint16_t animation_step;
	bool on;
};

void rgb_extra_start_transition_animation();
void zmk_rgb_underglow_tick_extra(struct k_work *work);
int zmk_rgb_underglow_set_hsb(struct zmk_led_hsb color);
int zmk_rgb_underglow_set_hue(int value);
int zmk_rgb_underglow_set_sat(int value);
int zmk_rgb_underglow_set_brt(int value);
int zmk_rgb_underglow_set_spd(int value);
struct rgb_underglow_state_extra *zmk_rgb_underglow_return_state();
