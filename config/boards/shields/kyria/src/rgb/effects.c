#include "rgb_extra.c"

enum rgb_effect {
	RGB_EFFECT_SOLID,
	RGB_EFFECT_BREATHE,
	RGB_EFFECT_SPECTRUM,
	RGB_EFFECT_SWIRL,
	UNDERGLOW_EFFECT_SPARKLE,
	UNDERGLOW_EFFECT_TEST,

	RGB_EFFECT_NUMBER // Used to track number of underglow effects
};

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

static void rgb_effect_solid() {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		pixels[i] = hsb_to_rgb(hsb_scale_min_max(state.color));
	}
}

static void rgb_effect_breathe() {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct zmk_led_hsb hsb = state.color;
		hsb.b = abs(state.animation_step - 1200) / 12;

		pixels[i] = hsb_to_rgb(hsb_scale_zero_max(hsb));
	}

	state.animation_step += state.animation_speed * 10;

	if (state.animation_step > 2400) {
		state.animation_step = 0;
	}
}

static void rgb_effect_spectrum() {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct zmk_led_hsb hsb = state.color;
		hsb.h = state.animation_step;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	state.animation_step += state.animation_speed;
	state.animation_step = state.animation_step % HUE_MAX;
}

static void rgb_effect_swirl() {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct zmk_led_hsb hsb = state.color;
		hsb.h = (HUE_MAX / STRIP_NUM_PIXELS * i + state.animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	state.animation_step += state.animation_speed * 2;
	state.animation_step = state.animation_step % HUE_MAX;
}

static void rgb_effect_sparkle() {
	struct zmk_led_hsb hsb = state.color;
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {

		int hue_offset = i * 69691;
		hsb.h = (hue_offset + state.animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	state.animation_step += state.animation_speed;
	state.animation_step = state.animation_step % HUE_MAX;
}

static void rgb_effect_solid_rainbow() {
	int leds = 31;
	// underglow: 0 - 5
	// per key: 6 - 30

	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct zmk_led_hsb hsb = state.color;
		hsb.b = 0;
		pixels[i] = hsb_to_rgb(hsb_scale_zero_max(hsb));
	}

	struct zmk_led_hsb hsb = state.color;
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

void rgb_effect_test() { rgb_effect_solid_rainbow(); }

void rgb_set_effect() {
	switch (state.current_effect) {
	case UNDERGLOW_EFFECT_SOLID:
		rgb_effect_solid();
		break;
	case UNDERGLOW_EFFECT_BREATHE:
		rgb_effect_breathe();
		break;
	case UNDERGLOW_EFFECT_SPECTRUM:
		rgb_effect_spectrum();
		break;
	case UNDERGLOW_EFFECT_SWIRL:
		rgb_effect_swirl();
		break;
	case UNDERGLOW_EFFECT_SPARKLE:
		rgb_effect_sparkle();
		break;
	case UNDERGLOW_EFFECT_TEST:
		rgb_effect_test();
		break;
	}
}
