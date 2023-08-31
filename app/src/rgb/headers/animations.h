#pragma once

#include "types.h"

void rgb_backlight_keypress_lightup_event_handler(const zmk_event_t *eh);
void rgb_backlight_update_keypress_effect_pixels();

void rgb_underglow_animation_set_pixels(int effect, rgb_strip_float pixels);

void rgb_backlight_animation_set_pixels(int effect, rgb_strip_float pixels);
void rgb_backlight_start_transition_effect(int duration_ms);
void rgb_backlight_transition_step();

// clang-format off
void rgb_backlight_effect_off             (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_solid        (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_breathe      (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_spectrum     (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_swirl        (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_sparkle      (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_solid_rainbow(rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_test         (rgb_strip_float pixels, int start, int end);
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
