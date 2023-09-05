#include <zmk/events/position_state_changed.h>
#include "rgb/rgb_backlight.h"
#include "imports.h"

static int keypress_lightup_state[STRIP_NUM_PIXELS];
// static int TRANSITION_STEPS = CONFIG_RGB_TRANSITION_DURATION / CONFIG_RGB_REFRESH_MS;
static int total_steps;

// int calculate_effect_steps(int duration_ms) { return duration_ms / CONFIG_RGB_REFRESH_MS; }

void rgb_backlight_start_transition_effect(int duration_ms) {
	duration_ms = duration_ms > 0 ? duration_ms : CONFIG_RGB_TRANSITION_DURATION;
	total_steps = duration_ms / CONFIG_RGB_REFRESH_MS;
	rgb_states.base.transition_steps_left = total_steps;

	copy_pixel_array(rgb_pixels.active, rgb_pixels.transition_start);
}

float interp(float start, float end, float ratio) { //
	return start + ((end - start) * ratio);
}

void rgb_backlight_transition_step() {
	float progress = (float)(total_steps - rgb_states.base.transition_steps_left) / total_steps;

	rgb_states.base.transition_steps_left--;
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		rgb_pixels.active[i] = (struct led_rgba){
			.r = interp(rgb_pixels.transition_start[i].r, rgb_pixels.transition_end[i].r, progress),
			.g = interp(rgb_pixels.transition_start[i].g, rgb_pixels.transition_end[i].g, progress),
			.b = interp(rgb_pixels.transition_start[i].b, rgb_pixels.transition_end[i].b, progress),
			.a = 1,
		};
	}
}
