#pragma once

#include <zmk/event_manager.h>
#include "types.h"

int layer_color_init();
void rgb_backlight_update_layer_color();

void rgb_backlight_keypress_lightup_event_handler(const zmk_event_t *eh);
void rgb_backlight_keypress_ripple_event_handler(const zmk_event_t *eh);
void rgb_backlight_update_keypress_effect_pixels();
void rgb_backlight_update_ripple_effect_pixels();
void change_keypress_pixel(struct led_rgba *pixel, float value);
float keypress_effect_calc_max_brightness();


void rgb_underglow_set_animation_pixels(int effect, rgba_strip pixels);

void rgb_backlight_set_animation_pixels(int effect, rgba_strip pixels);
void rgb_backlight_start_transition_effect(int duration_ms);
void rgb_backlight_transition_step();

// clang-format off
void rgb_backlight_effect_off             (rgba_strip pixels, int start, int end);
void rgb_backlight_animation_solid        (rgba_strip pixels, int start, int end);
// clang-format on

enum rgb_backlight_effects {
	RGB_BACKLIGHT_ANIMATION_OFF,
	RGB_BACKLIGHT_ANIMATION_SOLID,
	RGB_BACKLIGHT_ANIMATION_BREATHE,
	RGB_BACKLIGHT_ANIMATION_SPECTRUM,
	RGB_BACKLIGHT_ANIMATION_SWIRL,
	RGB_BACKLIGHT_ANIMATION_SPARKLE,
	RGB_BACKLIGHT_ANIMATION_TEST,

	RGB_BACKLIGHT_EFFECT_NUMBER // Used to track number of effects
};

enum rgb_underglow_effects {
	RGB_UNDERGLOW_ANIMATION_OFF,
	RGB_UNDERGLOW_ANIMATION_COPY,
	RGB_UNDERGLOW_ANIMATION_SOLID,
	RGB_UNDERGLOW_ANIMATION_NUMBER // Used to track number of effects
};

// float left_positions[25][2];
// float right_positions[25][2];