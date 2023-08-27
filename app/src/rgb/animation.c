#include "rgb_backlight.h"
#include "../display/widgets/headers/debug_output.h"
#include "../utils.h"

//---------------------------------------------Effects----------------------------------------------

void set_frame() {
	switch (rgb_state.current_effect) {
	case RGB_BACKLIGHT_EFFECT_SOLID:
		rgb_backlight_effect_solid();
		break;
	case RGB_BACKLIGHT_EFFECT_BREATHE:
		rgb_backlight_effect_breathe();
		break;
	case RGB_BACKLIGHT_EFFECT_SPECTRUM:
		rgb_backlight_effect_spectrum();
		break;
	case RGB_BACKLIGHT_EFFECT_SWIRL:
		rgb_backlight_effect_swirl();
		break;
	case RGB_BACKLIGHT_EFFECT_SPARKLE:
		rgb_backlight_effect_sparkle();
		break;
	case RGB_BACKLIGHT_EFFECT_TEST:
		rgb_backlight_effect_test();
		break;
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

void rgb_backlight_start_transition_animation() {
	struct led_rgb *initial_pixels = rgb_state.steps_left == 0 ? pixels : rgb_state.pixels;

	copy_array(initial_pixels, rgb_state.initial_pixels);

	rgb_state.total_steps = ANIMATION_DURATION / ANIMATION_REFRESH;
	rgb_state.steps_left = rgb_state.total_steps;
}

void rgb_backlight_transition_step() {
	double transition_progress =
		(double)(rgb_state.total_steps - rgb_state.steps_left) / rgb_state.total_steps;

	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct led_rgb start = rgb_state.initial_pixels[i];
		struct led_rgb end = pixels[i];

		int r = interpolate(start.r, end.r, transition_progress);
		int g = interpolate(start.g, end.g, transition_progress);
		int b = interpolate(start.b, end.b, transition_progress);

		rgb_state.pixels[i].r = r;
		rgb_state.pixels[i].g = g;
		rgb_state.pixels[i].b = b;
	}
}

void set_pixels(struct led_rgb *next_frame) {
	int err = led_strip_update_rgb(led_strip, next_frame, STRIP_NUM_PIXELS);
	// if (err < 0)
	// 	LOG_ERR("Failed to update the RGB strip (%d)", err);
}

void rgb_backlight_tick(struct k_work *work) {
	struct led_rgb *next_frame = pixels;

	set_frame();

	if (rgb_state.steps_left > 0) {
		rgb_state.animation_step--;
		rgb_backlight_transition_step();
		rgb_state.steps_left--;
		next_frame = rgb_state.pixels;
	}

	set_pixels(next_frame);
}
