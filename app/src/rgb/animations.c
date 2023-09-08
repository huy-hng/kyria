#include "rgb/rgb_backlight.h"

void rgb_backlight_effect_off(struct rgb_backlight_mode *state) {
	for (int i = state->range.start; i < state->range.end; i++) {
		set_pixel_white(&state->pixels[i], 0, true);
	}
}

void rgb_backlight_animation_solid(struct rgb_backlight_mode *state) {
	for (int i = state->range.start; i < state->range.end; i++) {
		state->pixels[i] = hsb_to_rgb(hsb_scale_min_max(state->color));
	}
}

void rgb_backlight_animation_breathe(struct rgb_backlight_mode *state) {
	for (int i = state->range.start; i < state->range.end; i++) {
		struct led_hsb hsb = state->color;
		hsb.b = abs(state->animation_step - 1200) / 12;

		state->pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	state->animation_step += state->animation_speed * 5;

	if (state->animation_step > 2400) {
		state->animation_step = 0;
	}
}

void rgb_backlight_animation_spectrum(struct rgb_backlight_mode *state) {
	for (int i = state->range.start; i < state->range.end; i++) {
		struct led_hsb hsb = state->color;
		hsb.h = state->animation_step;

		state->pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	state->animation_step += state->animation_speed;
	state->animation_step = state->animation_step % HUE_MAX;
}

void rgb_backlight_animation_swirl(struct rgb_backlight_mode *state) {
	for (int i = state->range.start; i < state->range.end; i++) {
		struct led_hsb hsb = state->color;
		int diff = state->range.end - state->range.start;
		hsb.h = (HUE_MAX / diff * i + state->animation_step) % HUE_MAX;

		state->pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	state->animation_step += state->animation_speed;
	state->animation_step = state->animation_step % HUE_MAX;
}

void rgb_backlight_animation_sparkle(struct rgb_backlight_mode *state) {
	struct led_hsb hsb = state->color;
	for (int i = state->range.start; i < state->range.end; i++) {
		int hue_offset = i * 69691;
		hsb.h = (hue_offset + state->animation_step) % HUE_MAX;

		state->pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	state->animation_step += state->animation_speed;
	state->animation_step = state->animation_step % HUE_MAX;
}

void rgb_backlight_animation_solid_rainbow(struct rgb_backlight_mode *state) {
	// int leds = 31;
	// underglow: 0 - 5
	// per key: 6 - 30

	struct led_hsb hsb = state->color;
	for (int i = 0; i < 6; i++) {
		hsb.h = (i * 60) % HUE_MAX;
		state->pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	for (int i = 6; i < 11; i++) {
		hsb.h = 90;
		state->pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	for (int i = 11; i < 19; i++) {
		hsb.h = 180;
		state->pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	for (int i = 19; i < 25; i++) {
		hsb.h = 270;
		state->pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	for (int i = 25; i < 31; i++) {
		hsb.h = 360;
		state->pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}
}

void rgb_backlight_animation_test(struct rgb_backlight_mode *state) { //
	rgb_backlight_animation_solid_rainbow(state);
}

//-----------------------------------------underglow----------------------------------------

// clang-format off
int left_pixels[25] = {
	30, 29, 28, 27, 26, 25,
	24, 23, 22, 21, 20, 19,
	18, 17, 16, 15, 14, 13, 12, 11,
				10,  9,  8,  7,  6
};
int left_underglow[6] = {
	2, 1, 0,
	3, 4, 5
};
int right_pixels[25] = {
			25, 26, 27, 28, 29, 30,
			19, 20, 21, 22, 23, 24,
	11, 12, 13, 14, 15, 16, 17, 18,
	 6,  7,  8,  9, 10
};
int right_underglow[6] = {
	0, 1, 2,
	5, 4, 3
};
// clang-format on

// first value is num of pixels because i dont know how to code
int i_0[] = {1, 25};
int i_1[] = {2, 26, 27};
int i_2[] = {1, 29};
int i_3[] = {2, 10, 17};
int i_4[] = {2, 8, 9};
int i_5[] = {2, 6, 11};
int *right_underglow_lookup[6] = {i_0 + 1, i_1 + 1, i_2 + 1, i_3 + 1, i_4 + 1, i_5 + 1};

static struct led_rgba average_pixels(rgba_strip pixels, int underglow_index) {
	int *indices = right_underglow_lookup[underglow_index];
	int arr_len = indices[-1];

	float r = 0, g = 0, b = 0;

	struct led_rgba pixel;

	for (int i = 0; i < arr_len; i++) {
		pixel = pixels[indices[i]];
		r += pixel.r * pixel.r;
		g += pixel.g * pixel.g;
		b += pixel.b * pixel.b;
	}

	return (struct led_rgba){
		.r = sqrtf(r / arr_len),
		.g = sqrtf(g / arr_len),
		.b = sqrtf(b / arr_len),
		.a = 1,
	};
}

static void copy_overglow(struct rgb_backlight_mode *state) {
	for (int i = 0; i < 6; i++) {
		state->pixels[i] = average_pixels(rgb_modes[rgb_mode_base].pixels, i);
	}
}

void rgb_backlight_set_animation_pixels(struct rgb_backlight_mode *state) {
	switch (state->active_animation) {
	case RGB_BACKLIGHT_ANIMATION_OFF:
		return rgb_backlight_effect_off(state);
	case RGB_BACKLIGHT_ANIMATION_SOLID:
		return rgb_backlight_animation_solid(state);
	case RGB_BACKLIGHT_ANIMATION_BREATHE:
		return rgb_backlight_animation_breathe(state);
	case RGB_BACKLIGHT_ANIMATION_SPECTRUM:
		return rgb_backlight_animation_spectrum(state);
	case RGB_BACKLIGHT_ANIMATION_SWIRL:
		return rgb_backlight_animation_swirl(state);
	case RGB_BACKLIGHT_ANIMATION_SPARKLE:
		return rgb_backlight_animation_sparkle(state);
	case RGB_BACKLIGHT_ANIMATION_TEST:
		return rgb_backlight_animation_test(state);

	case RGB_UNDERGLOW_ANIMATION_COPY:
		return copy_overglow(state);

		// int before = rgb_states.base.color.b;
		// rgb_states.base.color.b = before * 1.3;
		// rgb_states.base.color.b = before;
	}
}
