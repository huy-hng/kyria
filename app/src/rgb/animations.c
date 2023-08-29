#include "rgb_backlight.h"

struct zmk_led_hsb hsb_scale_min_max(struct zmk_led_hsb hsb);
struct zmk_led_hsb hsb_scale_zero_max(struct zmk_led_hsb hsb);
struct led_rgb hsb_to_rgb(struct zmk_led_hsb hsb);

//---------------------------------------------Effects----------------------------------------------

void rgb_backlight_animation_solid(rgb_strip pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		pixels[i] = hsb_to_rgb(hsb_scale_min_max(rgb_state.color));
	}
}

void rgb_backlight_animation_breathe(rgb_strip pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		struct zmk_led_hsb hsb = rgb_state.color;
		hsb.b = abs(rgb_state.animation_step - 1200) / 12;

		pixels[i] = hsb_to_rgb(hsb_scale_zero_max(hsb));
	}

	rgb_state.animation_step += rgb_state.animation_speed * 5;

	if (rgb_state.animation_step > 2400) {
		rgb_state.animation_step = 0;
	}
}

void rgb_backlight_animation_spectrum(rgb_strip pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		struct zmk_led_hsb hsb = rgb_state.color;
		hsb.h = rgb_state.animation_step;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_state.animation_step += rgb_state.animation_speed;
	rgb_state.animation_step = rgb_state.animation_step % HUE_MAX;
}

void rgb_backlight_animation_swirl(rgb_strip pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		struct zmk_led_hsb hsb = rgb_state.color;
		hsb.h = (HUE_MAX / (end - start) * i + rgb_state.animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_state.animation_step += rgb_state.animation_speed * 2;
	rgb_state.animation_step = rgb_state.animation_step % HUE_MAX;
}

void rgb_backlight_animation_sparkle(rgb_strip pixels, int start, int end) {
	struct zmk_led_hsb hsb = rgb_state.color;
	for (int i = start; i < end; i++) {
		int hue_offset = i * 69691;
		hsb.h = (hue_offset + rgb_state.animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_state.animation_step += rgb_state.animation_speed;
	rgb_state.animation_step = rgb_state.animation_step % HUE_MAX;
}

void rgb_backlight_animation_solid_rainbow(rgb_strip pixels, int start, int end) {
	int leds = 31;
	// underglow: 0 - 5
	// per key: 6 - 30

	struct zmk_led_hsb hsb = rgb_state.color;
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

void rgb_backlight_animation_test(rgb_strip pixels, int start, int end) { //
	rgb_backlight_animation_solid_rainbow(pixels, start, end);
}

//---------------------------------------------Helpers----------------------------------------------

void rgb_backlight_animation_set_pixels(int animation, rgb_strip pixels) {
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

struct zmk_led_hsb hsb_scale_min_max(struct zmk_led_hsb hsb) {
	hsb.b = CONFIG_ZMK_RGB_UNDERGLOW_BRT_MIN +
			(CONFIG_ZMK_RGB_UNDERGLOW_BRT_MAX - CONFIG_ZMK_RGB_UNDERGLOW_BRT_MIN) * hsb.b / BRT_MAX;
	return hsb;
}

struct zmk_led_hsb hsb_scale_zero_max(struct zmk_led_hsb hsb) {
	hsb.b = hsb.b * CONFIG_ZMK_RGB_UNDERGLOW_BRT_MAX / BRT_MAX;
	return hsb;
}

struct led_rgb hsb_to_rgb(struct zmk_led_hsb hsb) {
	float r, g, b;

	uint8_t i = hsb.h / 60;
	float v = hsb.b / ((float)BRT_MAX);
	float s = hsb.s / ((float)SAT_MAX);
	float f = hsb.h / ((float)HUE_MAX) * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (i % 6) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
		r = v;
		g = p;
		b = q;
		break;
	}

	struct led_rgb rgb = {
		.r = r * 255,
		.g = g * 255,
		.b = b * 255,
	};

	return rgb;
}
