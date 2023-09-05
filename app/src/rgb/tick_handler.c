#include "rgb/rgb_backlight.h"

void add_to_pixels(rgba_strip pixels, rgba_strip addition) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		pixels[i].r = pixels[i].r + addition[i].r;
		pixels[i].g = pixels[i].g + addition[i].g;
		pixels[i].b = pixels[i].b + addition[i].b;
	}
}

void set_pixels(rgba_strip pixels) {
	if (!rgb_states.base.on) {
		rgb_backlight_effect_off(pixels, 0, STRIP_NUM_PIXELS);
		return;
	}
	rgb_backlight_set_animation_pixels(rgb_states.base.current_effect, pixels);
	rgb_underglow_set_animation_pixels(rgb_states.underglow.current_effect, pixels);

	// rgb_backlight_update_keypress_effect_pixels();
	rgb_backlight_update_ripple_effect_pixels();
	add_to_pixels(pixels, rgb_pixels.keypress);
}

void apply_pixels(rgba_strip pixels) {
	rgb_strip rgb_int;
	rgb_strip_float_2_rgb_strip(pixels, rgb_int);
	int err = led_strip_update_rgb(led_strip, rgb_int, STRIP_NUM_PIXELS);

	if (err < 0) {
		// LOG_ERR("Failed to update the RGB strip (%d)", err);
	}
}

void rgb_backlight_tick(struct k_work *work) {
	if (rgb_states.base.transition_steps_left > 0) {
		set_pixels(rgb_pixels.transition_end);
		rgb_backlight_transition_step();
	} else {
		set_pixels(rgb_pixels.active);
		if (!rgb_states.base.on)
			rgb_backlight_stop();
	}

	apply_pixels(rgb_pixels.active);
}
