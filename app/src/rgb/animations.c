#include "rgb_backlight.h"

struct led_hsb hsb_scale_min_max(struct led_hsb hsb);
struct led_hsb hsb_scale_zero_max(struct led_hsb hsb);
struct led_rgb_float hsb_to_rgb(struct led_hsb hsb);

void rgb_backlight_animation_set_pixels(int animation, rgb_strip_float pixels) {
	switch (animation) {
	case RGB_BACKLIGHT_ANIMATION_SOLID:
		return rgb_backlight_animation_solid(pixels, OVERGLOW_INDEX_START, STRIP_NUM_PIXELS);
	case RGB_BACKLIGHT_ANIMATION_BREATHE:
		return rgb_backlight_animation_breathe(pixels, OVERGLOW_INDEX_START, STRIP_NUM_PIXELS);
	case RGB_BACKLIGHT_ANIMATION_SPECTRUM:
		return rgb_backlight_animation_spectrum(pixels, OVERGLOW_INDEX_START, STRIP_NUM_PIXELS);
	case RGB_BACKLIGHT_ANIMATION_SWIRL:
		return rgb_backlight_animation_swirl(pixels, OVERGLOW_INDEX_START, STRIP_NUM_PIXELS);
	case RGB_BACKLIGHT_ANIMATION_SPARKLE:
		return rgb_backlight_animation_sparkle(pixels, OVERGLOW_INDEX_START, STRIP_NUM_PIXELS);
	case RGB_BACKLIGHT_ANIMATION_TEST:
		return rgb_backlight_animation_test(pixels, OVERGLOW_INDEX_START, STRIP_NUM_PIXELS);
	}
}

//---------------------------------------------Effects----------------------------------------------

void rgb_backlight_effect_off(rgb_strip_float pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		set_pixel_white(&pixels[i], 0, true);
	}
}

void rgb_backlight_animation_solid(rgb_strip_float pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		pixels[i] = hsb_to_rgb(hsb_scale_min_max(rgb_states.active->color));
	}
}

void rgb_backlight_animation_breathe(rgb_strip_float pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		struct led_hsb hsb = rgb_states.active->color;
		hsb.b = abs(rgb_states.active->animation_step - 1200) / 12;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_states.active->animation_step += rgb_states.active->animation_speed * 5;

	if (rgb_states.active->animation_step > 2400) {
		rgb_states.active->animation_step = 0;
	}
}

void rgb_backlight_animation_spectrum(rgb_strip_float pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		struct led_hsb hsb = rgb_states.active->color;
		hsb.h = rgb_states.active->animation_step;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_states.active->animation_step += rgb_states.active->animation_speed;
	rgb_states.active->animation_step = rgb_states.active->animation_step % HUE_MAX;
}

void rgb_backlight_animation_swirl(rgb_strip_float pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		struct led_hsb hsb = rgb_states.active->color;
		hsb.h = (HUE_MAX / (end - start) * i + rgb_states.active->animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_states.active->animation_step += rgb_states.active->animation_speed;
	rgb_states.active->animation_step = rgb_states.active->animation_step % HUE_MAX;
}

void rgb_backlight_animation_sparkle(rgb_strip_float pixels, int start, int end) {
	struct led_hsb hsb = rgb_states.active->color;
	for (int i = start; i < end; i++) {
		int hue_offset = i * 69691;
		hsb.h = (hue_offset + rgb_states.active->animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_states.active->animation_step += rgb_states.active->animation_speed;
	rgb_states.active->animation_step = rgb_states.active->animation_step % HUE_MAX;
}

void rgb_backlight_animation_solid_rainbow(rgb_strip_float pixels, int start, int end) {
	int leds = 31;
	// underglow: 0 - 5
	// per key: 6 - 30

	struct led_hsb hsb = rgb_states.active->color;
	for (int i = 0; i < 6; i++) {
		hsb.h = (i * 60) % HUE_MAX;
		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	for (int i = 6; i < 11; i++) {
		hsb.h = 90;
		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	for (int i = 11; i < 19; i++) {
		hsb.h = 180;
		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	for (int i = 19; i < 25; i++) {
		hsb.h = 270;
		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	for (int i = 25; i < 31; i++) {
		hsb.h = 360;
		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}
}

void rgb_backlight_animation_test(rgb_strip_float pixels, int start, int end) { //
	rgb_backlight_animation_solid_rainbow(pixels, start, end);
}
