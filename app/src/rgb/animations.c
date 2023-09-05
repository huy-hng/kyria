#include "rgb/rgb_backlight.h"

void rgb_backlight_effect_off(rgba_strip pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		set_pixel_white(&pixels[i], 0, true);
	}
}

void rgb_backlight_animation_solid(rgba_strip pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		pixels[i] = hsb_to_rgb(hsb_scale_min_max(rgb_states.active->color));
	}
}

void rgb_backlight_animation_breathe(rgba_strip pixels, int start, int end) {
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

void rgb_backlight_animation_spectrum(rgba_strip pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		struct led_hsb hsb = rgb_states.active->color;
		hsb.h = rgb_states.active->animation_step;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_states.active->animation_step += rgb_states.active->animation_speed;
	rgb_states.active->animation_step = rgb_states.active->animation_step % HUE_MAX;
}

void rgb_backlight_animation_swirl(rgba_strip pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		struct led_hsb hsb = rgb_states.active->color;
		hsb.h = (HUE_MAX / (end - start) * i + rgb_states.active->animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_states.active->animation_step += rgb_states.active->animation_speed;
	rgb_states.active->animation_step = rgb_states.active->animation_step % HUE_MAX;
}

void rgb_backlight_animation_sparkle(rgba_strip pixels, int start, int end) {
	struct led_hsb hsb = rgb_states.active->color;
	for (int i = start; i < end; i++) {
		int hue_offset = i * 69691;
		hsb.h = (hue_offset + rgb_states.active->animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_states.active->animation_step += rgb_states.active->animation_speed;
	rgb_states.active->animation_step = rgb_states.active->animation_step % HUE_MAX;
}

void rgb_backlight_animation_solid_rainbow(rgba_strip pixels, int start, int end) {
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

void rgb_backlight_animation_test(rgba_strip pixels, int start, int end) { //
	rgb_backlight_animation_solid_rainbow(pixels, start, end);
}

void rgb_backlight_set_animation_pixels(int animation, rgba_strip pixels) {
	switch (animation) {
	case RGB_BACKLIGHT_ANIMATION_OFF:
		return rgb_backlight_effect_off(pixels, OVERGLOW_INDEX_START, STRIP_NUM_PIXELS);
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
