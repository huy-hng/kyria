#pragma once

#include <zmk/event_manager.h>
#include "types.h"

int layer_color_init();
void rgb_backlight_layer_color_event_handler(uint8_t index);
void rgb_backlight_set_layer_color(uint8_t active_layer_index);


void rgb_backlight_keypress_lightup_event_handler(const zmk_event_t *eh);
void rgb_backlight_keypress_ripple_event_handler(const zmk_event_t *eh);
void rgb_backlight_update_keypress_effect_pixels();
void rgb_backlight_update_ripple_effect_pixels();
void change_keypress_pixel(struct led_rgba *pixel, float value);
float keypress_effect_calc_max_brightness();

void rgb_underglow_set_animation_pixels(int effect, rgba_strip pixels);

void rgb_backlight_set_animation_pixels(struct rgb_backlight_mode *state);
void rgb_backlight_start_transition_effect(int duration_ms);
void rgb_backlight_transition_step();

void rgb_backlight_effect_off(struct rgb_backlight_mode *state);
void rgb_backlight_animation_solid(struct rgb_backlight_mode *state);

enum rgb_backlight_effects {
	RGB_BACKLIGHT_ANIMATION_OFF,
	RGB_BACKLIGHT_ANIMATION_SOLID,
	RGB_BACKLIGHT_ANIMATION_BREATHE,
	RGB_BACKLIGHT_ANIMATION_SPECTRUM,
	RGB_BACKLIGHT_ANIMATION_SWIRL,
	RGB_BACKLIGHT_ANIMATION_SPARKLE,

	RGB_BACKLIGHT_EFFECT_NUMBER, // Used to track number of effects

	RGB_UNDERGLOW_ANIMATION_COPY,
};

// int left_pixels[25] = {
// 	30, 29, 28, 27, 26, 25,
// 	24, 23, 22, 21, 20, 19,
// 	18, 17, 16, 15, 14, 13, 12, 11,
// 				10,  9,  8,  7,  6
// };
// int left_underglow[6] = {
// 	2, 1, 0,
// 	3, 4, 5
// };
// int right_pixels[25] = {
// 			25, 26, 27, 28, 29, 30,
// 			19, 20, 21, 22, 23, 24,
// 	11, 12, 13, 14, 15, 16, 17, 18,
// 	 6,  7,  8,  9, 10
// };
// int right_underglow[6] = {
// 	0, 1, 2,
// 	5, 4, 3
// };
