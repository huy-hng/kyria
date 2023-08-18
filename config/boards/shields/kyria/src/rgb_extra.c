#include "rgb_underglow.c"
#include "rgb_extra.h"
#include "display/widgets/headers/debug_output.h"
#include <stdio.h>

enum rgb_underglow_effect_extra {
	UNDERGLOW_EFFECT_SPARKLE = 4,
	UNDERGLOW_EFFECT_TEST,
};

struct rgb_value {
	int r;
	int g;
	int b;
};

struct rgb_animation_state {
	struct zmk_led_hsb target_color;
	struct led_rgb initial_pixels[STRIP_NUM_PIXELS];
	struct led_rgb pixels[STRIP_NUM_PIXELS];
	struct rgb_value pixel_step[STRIP_NUM_PIXELS];
	int steps_left;
	int animation_time;
	int total_steps;
	bool transitioning;
} anim_state;

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

static void rgb_extra_effect_sparkle() {
	struct zmk_led_hsb hsb = state.color;
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {

		int hue_offset = i * 69691;
		hsb.h = (hue_offset + state.animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	state.animation_step += state.animation_speed;
	state.animation_step = state.animation_step % HUE_MAX;
}

static void rgb_extra_effect_solid_rainbow() {
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

void rgb_extra_effect_test() { rgb_extra_effect_solid_rainbow(); }

//---------------------------------------

void set_pixels(struct led_rgb *next_frame) {
	int err = led_strip_update_rgb(led_strip, next_frame, STRIP_NUM_PIXELS);
	if (err < 0)
		LOG_ERR("Failed to update the RGB strip (%d)", err);
}

void set_frame() {
	switch (state.current_effect) {
	case UNDERGLOW_EFFECT_SOLID:
		zmk_rgb_underglow_effect_solid();
		break;
	case UNDERGLOW_EFFECT_BREATHE:
		zmk_rgb_underglow_effect_breathe();
		break;
	case UNDERGLOW_EFFECT_SPECTRUM:
		zmk_rgb_underglow_effect_spectrum();
		break;
	case UNDERGLOW_EFFECT_SWIRL:
		zmk_rgb_underglow_effect_swirl();
		break;
	case UNDERGLOW_EFFECT_SPARKLE:
		rgb_extra_effect_sparkle();
		break;
	case UNDERGLOW_EFFECT_TEST:
		rgb_extra_effect_test();
		break;
	}
}

void rgb_extra_effect_transition() {
	struct zmk_led_hsb hsb = state.color;
	struct led_rgb target_rgb = hsb_to_rgb(hsb_scale_min_max(hsb));

	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct led_rgb *pixel = &pixels[i];

		float r = (float)(target_rgb.r - pixel->r) / state.animation_step;
		float g = (float)(target_rgb.g - pixel->g) / state.animation_step;
		float b = (float)(target_rgb.b - pixel->b) / state.animation_step;

		if (r == 0 && g == 0 && b == 0) {
			// state.current_effect = 4;
		}

		// pixel->r += CLAMP(r, 0, 255);
		// pixel->g += CLAMP(g, 0, 255);
		// pixel->b += CLAMP(b, 0, 255);

		pixel->r += r;
		pixel->g += g;
		pixel->b += b;
	}
}

void copy_array(struct led_rgb *arr1, struct led_rgb *arr2) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct led_rgb *pixel = &arr1[i];

		arr2[i] = (struct led_rgb){
			.r = pixel->r,
			.g = pixel->g,
			.b = pixel->b,
		};
	}
}

int interpolate(int start, int end, float ratio) { //
	return start + ((end - start) * ratio);
}

void rgb_extra_create_transition_array() {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct led_rgb *target_pixel = &pixels[i];
		struct led_rgb *initial_pixel = &anim_state.initial_pixels[i];

		anim_state.pixel_step[i] = (struct rgb_value){
			.r = (target_pixel->r - initial_pixel->r) / anim_state.steps_left,
			.g = (target_pixel->g - initial_pixel->g) / anim_state.steps_left,
			.b = (target_pixel->b - initial_pixel->b) / anim_state.steps_left,
		};
	}

	struct led_rgb target = pixels[11];
	struct led_rgb initial = anim_state.initial_pixels[11];
	struct rgb_value step = anim_state.pixel_step[11];
	// debug_set_text_fmt("%d %d %d", target.r, target.g, target.b);
	// debug_add_text_fmt("%d %d %d", initial.r, initial.g, initial.b);
	// debug_add_text_fmt("%d %d %d", step.r, step.g, step.b);

	// LOG_DBG("steps %d", anim_state.steps_left);
}

void rgb_extra_start_transition_animation() {
	copy_array(pixels, anim_state.initial_pixels);
	anim_state.total_steps = ANIMATION_DURATION / ANIMATION_REFRESH;
	anim_state.steps_left = anim_state.total_steps;

	// set next frame to calc pixel step
	set_frame();

	struct led_rgb start = anim_state.initial_pixels[11];
	// struct led_rgb current = anim_state.pixels[11];
	struct led_rgb end = pixels[11];
	int r = interpolate(start.r, end.r, 0.5);
	int g = interpolate(start.g, end.g, 0.5);
	int b = interpolate(start.b, end.b, 0.5);
	// debug_set_text_fmt("%d %d %d", start.r, start.g, start.b);
	// // debug_add_text_fmt("%d %d %d", current.r, current.g, current.b);
	// debug_add_text_fmt("%d %d %d", end.r, end.g, end.b);
	// debug_add_text_fmt("%d %d %d", r, g, b);
}

void rgb_extra_transition_step() {
	double transition_progress =
		(double)(anim_state.total_steps - anim_state.steps_left) / anim_state.total_steps;

	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct led_rgb start = anim_state.initial_pixels[i];
		struct led_rgb end = pixels[i];

		int r = interpolate(start.r, end.r, transition_progress);
		int g = interpolate(start.g, end.g, transition_progress);
		int b = interpolate(start.b, end.b, transition_progress);

		anim_state.pixels[i].r = r;
		anim_state.pixels[i].g = g;
		anim_state.pixels[i].b = b;
	}

	// debug_add_text_fmt("%d %d %d", current.r, current.g, current.b);
}

void zmk_rgb_underglow_tick_extra(struct k_work *work) {
	struct led_rgb *next_frame = pixels;

	set_frame();

	if (anim_state.steps_left >= 0) {
		rgb_extra_transition_step();
		anim_state.steps_left--;
		state.animation_step--;
		next_frame = anim_state.pixels;
	}

	// if (anim_state.steps_left == 18) {
	// 	struct led_rgb start = anim_state.initial_pixels[11];
	// 	struct led_rgb end = pixels[11];
	// 	struct led_rgb current = anim_state.pixels[11];
	// 	debug_set_text_fmt("%d %d %d", start.r, start.g, start.b);
	// 	debug_add_text_fmt("%d %d %d", current.r, current.g, current.b);
	// 	debug_add_text_fmt("%d %d %d", end.r, end.g, end.b);
	// }

	set_pixels(next_frame);
}

//-------------------------------------------------------

int zmk_rgb_underglow_set_hue(int value) {
	if (!led_strip)
		return -ENODEV;

	state.color.h = value % HUE_MAX;

	return zmk_rgb_underglow_save_state();
}

int zmk_rgb_underglow_set_sat(int value) {
	if (!led_strip)
		return -ENODEV;
	state.color.s = CLAMP(value, 0, SAT_MAX);

	return zmk_rgb_underglow_save_state();
}

int zmk_rgb_underglow_set_brt(int value) {
	if (!led_strip)
		return -ENODEV;

	state.color.b = CLAMP(value, 0, BRT_MAX);

	return zmk_rgb_underglow_save_state();
}

int zmk_rgb_underglow_set_spd(int value) {
	if (!led_strip)
		return -ENODEV;

	state.animation_speed = CLAMP(value, 1, 5);
	return zmk_rgb_underglow_save_state();
}

struct rgb_underglow_state_extra *zmk_rgb_underglow_return_state() {
	return (struct rgb_underglow_state_extra *)&state;
}
