#include "rgb_backlight.h"
#include "../imports.h"

rgb_strip pixels_start;
rgb_strip pixels_end;
static int TOTAL_STEPS = CONFIG_RGB_TRANSITION_DURATION / CONFIG_RGB_REFRESH_MS;
// static int TOTAL_STEPS = 1000 / CONFIG_RGB_REFRESH_MS;

int calculate_effect_steps(int duration_ms){
	return duration_ms / CONFIG_RGB_REFRESH_MS;
}

void copy_pixel_array(struct led_rgb *arr1, struct led_rgb *arr2) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++)
		arr2[i] = arr1[i];
}

void rgb_backlight_start_transition_effect() {
	copy_pixel_array(pixels, pixels_start);
	rgb_state.transition_steps_left = TOTAL_STEPS;
}

int interpolate(int start, int end, float ratio) { //
	return start + ((end - start) * ratio);
}

void rgb_backlight_transition_step() {
	double transition_progress =
		(double)(TOTAL_STEPS - rgb_state.transition_steps_left) / TOTAL_STEPS;

	rgb_state.transition_steps_left--;
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		pixels[i] = (struct led_rgb){
			.r = interpolate(pixels_start[i].r, pixels_end[i].r, transition_progress),
			.g = interpolate(pixels_start[i].g, pixels_end[i].g, transition_progress),
			.b = interpolate(pixels_start[i].b, pixels_end[i].b, transition_progress),
		};
	}
}

