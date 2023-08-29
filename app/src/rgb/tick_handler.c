#include <zmk/workqueue.h>

#include "rgb_backlight.h"

// REFACTOR:
void rgb_backlight_effect_off(rgb_strip pixels, int start, int end) {
	for (int i = start; i < end; i++) {
		pixels[i].r = 0;
		pixels[i].g = 0;
		pixels[i].b = 0;
	}
}

void set_pixels(struct led_rgb *pixels) {
	if (!rgb_state.on) {
		rgb_backlight_effect_off(pixels, 0, STRIP_NUM_PIXELS);
		return;
	}

	rgb_backlight_animation_set_pixels(rgb_state.current_effect, pixels);
	rgb_underglow_animation_set_pixels(rgb_state.underglow_effect, pixels);
}

void apply_pixels(struct led_rgb *pixels) {
	int err = led_strip_update_rgb(led_strip, pixels, STRIP_NUM_PIXELS);
	// if (err < 0)
	// 	LOG_ERR("Failed to update the RGB strip (%d)", err);
}

void rgb_backlight_tick(struct k_work *work) {
	if (rgb_state.transition_steps_left > 0) {
		set_pixels(pixels_end);
		rgb_backlight_transition_step();
	} else {
		set_pixels(pixels);
		if (!rgb_state.on)
			rgb_backlight_stop();
	}

	apply_pixels(pixels);
}

K_WORK_DEFINE(underglow_tick_work, rgb_backlight_tick);

static void rgb_backlight_tick_handler(struct k_timer *timer) {
	k_work_submit_to_queue(zmk_workqueue_lowprio_work_q(), &underglow_tick_work);
}

K_TIMER_DEFINE(backlight_tick, rgb_backlight_tick_handler, NULL);
