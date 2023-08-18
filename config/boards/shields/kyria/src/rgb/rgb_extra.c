#include "rgb_extra.h"
#include "rgb_underglow.c"

#include "../display/widgets/headers/debug_output.h"

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

void rgb_extra_start_transition_animation() {
	struct led_rgb *initial_pixels = anim_state.steps_left == 0 ? pixels : anim_state.pixels;

	copy_array(initial_pixels, anim_state.initial_pixels);

	anim_state.total_steps = ANIMATION_DURATION / ANIMATION_REFRESH;
	anim_state.steps_left = anim_state.total_steps;
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
}

void zmk_rgb_underglow_tick_extra(struct k_work *work) {
	struct led_rgb *next_frame = pixels;

	rgb_set_effect();

	if (anim_state.steps_left > 0) {
		state.animation_step--;
		rgb_extra_transition_step();
		anim_state.steps_left--;
		next_frame = anim_state.pixels;
	}
	// debug_set_text_fmt("%d", anim_state.steps_left);

	int err = led_strip_update_rgb(led_strip, next_frame, STRIP_NUM_PIXELS);
	if (err < 0)
		LOG_ERR("Failed to update the RGB strip (%d)", err);
}
