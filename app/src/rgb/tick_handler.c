#include <zmk/workqueue.h>
#include "rgb_backlight.h"
#include "zephyr/sys/util.h"

void add_to_pixels(rgb_strip_float pixels, rgb_strip_float addition) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		pixels[i].r = pixels[i].r + addition[i].r;
		pixels[i].g = pixels[i].g + addition[i].g;
		pixels[i].b = pixels[i].b + addition[i].b;
	}
}

void set_pixels(rgb_strip_float pixels) {
	if (!rgb_states.base.on) {
		rgb_backlight_effect_off(pixels, 0, STRIP_NUM_PIXELS);
		return;
	}
	rgb_backlight_animation_set_pixels(rgb_states.base.current_effect, pixels);
	rgb_underglow_animation_set_pixels(rgb_states.underglow.current_effect, pixels);
	rgb_backlight_update_keypress_effect_pixels();
	debug_set_text_fmt("r: %d", (int)(pixels[1].r * 255));
	debug_newline_text_fmt("g: %d", (int)(pixels[1].g * 255));
	debug_newline_text_fmt("b: %d", (int)(pixels[1].b * 255));
	add_to_pixels(pixels, rgb_pixels.keypress);
}

void apply_pixels(rgb_strip_float pixels) {
	rgb_strip rgb_8;
	rgb_strip_float_2_rgb_strip(pixels, rgb_8);
	int err = led_strip_update_rgb(led_strip, rgb_8, STRIP_NUM_PIXELS);
	// int err = led_strip_update_rgb(led_strip, pixels, STRIP_NUM_PIXELS);
	// if (err < 0)
	// 	LOG_ERR("Failed to update the RGB strip (%d)", err);
}

void rgb_backlight_tick(struct k_work *work) {
	if (rgb_states.base.transition_steps_left > 0) {
		// rgb_pixels.active = &rgb_pixels.transition_end;
		set_pixels(rgb_pixels.transition_end);
		rgb_backlight_transition_step();
	} else {
		set_pixels(rgb_pixels.active);
		if (!rgb_states.base.on)
			rgb_backlight_stop();
	}

	apply_pixels(rgb_pixels.active);
}

K_WORK_DEFINE(underglow_tick_work, rgb_backlight_tick);

static void rgb_backlight_tick_handler(struct k_timer *timer) {
	k_work_submit_to_queue(zmk_workqueue_lowprio_work_q(), &underglow_tick_work);
}

K_TIMER_DEFINE(backlight_tick, rgb_backlight_tick_handler, NULL);
