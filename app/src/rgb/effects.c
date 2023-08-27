#include "rgb_backlight.h"
#include <stdlib.h>

struct zmk_led_hsb hsb_scale_min_max(struct zmk_led_hsb hsb);
struct zmk_led_hsb hsb_scale_zero_max(struct zmk_led_hsb hsb);
struct led_rgb hsb_to_rgb(struct zmk_led_hsb hsb);

// clang-format off
int left_pixel_array[25] = {
	30, 29, 28, 27, 26, 25,
	24, 23, 22, 21, 20, 19,
	18, 17, 16, 15, 14, 13, 12, 11,
				10,  9,  8,  7,  6
};
int right_pixel_array[25] = {
			25, 26, 27, 28, 29, 30,
			19, 20, 21, 22, 23, 24,
	11, 12, 13, 14, 15, 16, 17, 18,
	 6,  7,  8,  9, 10
};
int right_underglow[6] = {
	0, 1, 2,
	5, 4, 3
};
int left_underglow[6] = {
	2, 1, 0,
	3, 4, 5
};
// clang-format on

//---------------------------------------------Effects----------------------------------------------

void rgb_backlight_effect_solid(/* rgb_strip pixels */) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		pixels[i] = hsb_to_rgb(hsb_scale_min_max(rgb_state.color));
	}
}

void rgb_backlight_effect_breathe(/* rgb_strip pixels */) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct zmk_led_hsb hsb = rgb_state.color;
		hsb.b = abs(rgb_state.animation_step - 1200) / 12;

		pixels[i] = hsb_to_rgb(hsb_scale_zero_max(hsb));
	}

	rgb_state.animation_step += rgb_state.animation_speed * 10;

	if (rgb_state.animation_step > 2400) {
		rgb_state.animation_step = 0;
	}
}

void rgb_backlight_effect_spectrum(/* rgb_strip pixels */) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct zmk_led_hsb hsb = rgb_state.color;
		hsb.h = rgb_state.animation_step;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_state.animation_step += rgb_state.animation_speed;
	rgb_state.animation_step = rgb_state.animation_step % HUE_MAX;
}

void rgb_backlight_effect_swirl(/* rgb_strip pixels */) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct zmk_led_hsb hsb = rgb_state.color;
		hsb.h = (HUE_MAX / STRIP_NUM_PIXELS * i + rgb_state.animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_state.animation_step += rgb_state.animation_speed * 2;
	rgb_state.animation_step = rgb_state.animation_step % HUE_MAX;
}

void rgb_backlight_effect_sparkle(/* rgb_strip pixels */) {
	struct zmk_led_hsb hsb = rgb_state.color;
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {

		int hue_offset = i * 69691;
		hsb.h = (hue_offset + rgb_state.animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	rgb_state.animation_step += rgb_state.animation_speed;
	rgb_state.animation_step = rgb_state.animation_step % HUE_MAX;
}

void rgb_backlight_effect_solid_rainbow(/* rgb_strip pixels */) {
	int leds = 31;
	// underglow: 0 - 5
	// per key: 6 - 30

	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct zmk_led_hsb hsb = rgb_state.color;
		hsb.b = 0;
		pixels[i] = hsb_to_rgb(hsb_scale_zero_max(hsb));
	}

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

void rgb_backlight_effect_test(/* rgb_strip pixels */) { //
	rgb_backlight_effect_solid_rainbow(/* pixels */);
}

//---------------------------------------------Helpers----------------------------------------------

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
