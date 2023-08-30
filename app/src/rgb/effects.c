#include <zmk/events/position_state_changed.h>
#include "rgb_backlight.h"
#include "../imports.h"

static int keypress_lightup_state[STRIP_NUM_PIXELS];
int TRANSITION_STEPS = CONFIG_RGB_TRANSITION_DURATION / CONFIG_RGB_REFRESH_MS;

int calculate_effect_steps(int duration_ms) { return duration_ms / CONFIG_RGB_REFRESH_MS; }

void rgb_backlight_start_transition_effect() {
	copy_pixel_array(rgb_pixels.active, rgb_pixels.transition_start);
	rgb_states.base.transition_steps_left = TRANSITION_STEPS;
}

float interpolate(float start, float end, float ratio) { //
	return start + ((end - start) * ratio);
}

void rgb_backlight_transition_step() {
	float transition_progress =
		(float)(TRANSITION_STEPS - rgb_states.base.transition_steps_left) / TRANSITION_STEPS;

	rgb_states.base.transition_steps_left--;
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		rgb_pixels.active[i] = (struct led_rgb_float){
			.r = interpolate(rgb_pixels.transition_start[i].r, rgb_pixels.transition_end[i].r, transition_progress),
			.g = interpolate(rgb_pixels.transition_start[i].g, rgb_pixels.transition_end[i].g, transition_progress),
			.b = interpolate(rgb_pixels.transition_start[i].b, rgb_pixels.transition_end[i].b, transition_progress),
		};
	}
}
