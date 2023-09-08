#include "rgb/rgb_backlight.h"
static bool off_on_next_tick = false;

static void add_to_pixels(rgba_strip pixels, rgba_strip addition) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		pixels[i].r = pixels[i].r + addition[i].r;
		pixels[i].g = pixels[i].g + addition[i].g;
		pixels[i].b = pixels[i].b + addition[i].b;
	}
}

static void replace_pixels(rgba_strip pixels, struct rgb_backlight_mode *state) {
	if (state->color.a == 0)
		return;
	for (int i = state->range.start; i < state->range.end; i++) {
		if (state->pixels[i].a == 0)
			continue;
		pixels[i].r = state->pixels[i].r;
		pixels[i].g = state->pixels[i].g;
		pixels[i].b = state->pixels[i].b;
	}
}

static float weighted_interp(float val1, float val2, float weight) {
	return sqrtf(SQUARE(val1) * (1 - weight) + SQUARE(val2) * weight);
}

float linear_interp(float start, float end, float ratio) { //
	return start + ((end - start) * ratio);
}

static void average_pixels(rgba_strip pixels, struct rgb_backlight_mode *state) {
	if (state->color.a == 0)
		return;

	float alpha = state->color.a / 100.0;
	for (int i = state->range.start; i < state->range.end; i++) {
		if (state->pixels[i].a == 0)
			continue;

		// sqrt(R1 ^ 2 * w + R2 ^ 2 * [1 - w]);
		// sqrt(G1 ^ 2 * w + G2 ^ 2 * [1 - w]);
		// sqrt(B1 ^ 2 * w + B2 ^ 2 * [1 - w]);

		pixels[i].r = linear_interp(pixels[i].r, state->pixels[i].r, alpha);
		pixels[i].g = linear_interp(pixels[i].g, state->pixels[i].g, alpha);
		pixels[i].b = linear_interp(pixels[i].b, state->pixels[i].b, alpha);

		// pixels[i].r = weighted_interp(pixels[i].r, state->pixels[i].r, alpha);
		// pixels[i].g = weighted_interp(pixels[i].g, state->pixels[i].g, alpha);
		// pixels[i].b = weighted_interp(pixels[i].b, state->pixels[i].b, alpha);
	}
}

static void blend_pixels(rgba_strip pixels) {}

void set_pixels(rgba_strip pixels) {
	debug_set_text_fmt("%d", (int)rgb_modes[rgb_mode_layer_color].color.a);
	if (rgb_modes[rgb_mode_base].on) {
		rgb_backlight_set_animation_pixels(&rgb_modes[rgb_mode_base]);
		rgb_backlight_set_animation_pixels(&rgb_modes[rgb_mode_underglow]);
		rgb_backlight_set_layer_color(active_layer_index);
	} else {
		off_on_next_tick = true;
		rgb_backlight_effect_off(&rgb_modes[rgb_mode_base]);
		rgb_backlight_effect_off(&rgb_modes[rgb_mode_underglow]);
	}

	replace_pixels(pixels, &rgb_modes[rgb_mode_base]);
	replace_pixels(pixels, &rgb_modes[rgb_mode_underglow]);
	average_pixels(pixels, &rgb_modes[rgb_mode_layer_color]);

	// rgb_backlight_update_keypress_effect_pixels();
	// rgb_backlight_update_ripple_effect_pixels();
	// add_to_pixels(pixels, rgb_states.key_react.pixels);
}

void apply_pixels(rgba_strip rgba_strip) {
	static rgb_strip rgb;
	rgb_strip_float_2_rgb_strip(rgba_strip, rgb);

	int err = led_strip_update_rgb(led_strip, rgb, STRIP_NUM_PIXELS);
	if (err < 0) {
		// LOG_ERR("Failed to update the RGB strip (%d)", err);
	}
}

void rgb_backlight_tick(struct k_work *work) {
	// if (rgb_states.base.transition_steps_left > 0) {
	// 	set_pixels(rgb_pixels.transition_end);
	// 	rgb_backlight_transition_step();
	// } else {
	// 	set_pixels(rgb_pixels.active);
	// 	if (!rgb_states.base.on)
	// 		rgb_backlight_stop();
	// }

	if (off_on_next_tick) {
		rgb_backlight_stop();
		off_on_next_tick = false;
		return;
	}
	rgb_modes[rgb_mode_base].color.a = 100;

	static rgba_strip strip;
	set_pixels(strip);
	apply_pixels(strip);
}
