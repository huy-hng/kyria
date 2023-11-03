#include "rgb/rgb_backlight.h"
typedef float blending_fn(float, float, float);

static rgba_strip combined_pixels;

// clang-format off
static float             add(float val1, float val2, float ratio) { return val1 + val2; }
static float            mask(float val1, float val2, float ratio) { return val1 * ratio; }
static float         replace(float val1, float val2, float ratio) { return val2; }
static float weighted_interp(float val1, float val2, float ratio) {
	return sqrtf(SQUARE(val1) * (1 - ratio) + SQUARE(val2) * ratio);
}
// clang-format on
static float linear_interp(float start, float end, float ratio) {
	return start + ((end - start) * ratio);
}

static void combine_pixels(rgba_strip pixels, struct rgb_backlight_mode *state, blending_fn fn) {
	float alpha = state->color.a / 100.0;
	for (int i = state->range.start; i < state->range.end; i++) {
		pixels[i].r = fn(pixels[i].r, state->pixels[i].r, alpha);
		pixels[i].g = fn(pixels[i].g, state->pixels[i].g, alpha);
		pixels[i].b = fn(pixels[i].b, state->pixels[i].b, alpha);
	}
}

static void handle_on_off_animation(rgba_strip combined_pixels) {
	float step_size = ((float)CONFIG_RGB_REFRESH_MS / TURN_OFF_DURATION);

	if (!rgb_modes[rgb_mode_base].on)
		step_size *= -1; // invert to subtract step_size

	// FIX: unnecessary to run each tick, only on on/off state change
	//      add an event handler or similar
	for (int i = 0; i < STRIP_NUM_PIXELS; i++)
		combined_pixels[i].a = CLAMP(combined_pixels[i].a + step_size, 0.0, 1.0);
}

static void set_mode_pixels(struct rgb_backlight_mode mode) {}

static void set_pixels(rgba_strip combined_pixels) {
	// TODO: everything below should be 'handled' by `struct rgb_backlight_mode`
	//       see more in neorg /home/huy/.dotfiles/personal/personal/neorg/projects/zmk.norg
	rgb_backlight_set_animation_pixels(&rgb_modes[rgb_mode_base]);
	rgb_backlight_set_animation_pixels(&rgb_modes[rgb_mode_underglow]);
	rgb_backlight_set_layer_color(active_layer_index);

	combine_pixels(combined_pixels, &rgb_modes[rgb_mode_base], &replace);
	combine_pixels(combined_pixels, &rgb_modes[rgb_mode_underglow], &replace);
	combine_pixels(combined_pixels, &rgb_modes[rgb_mode_key_layer], &linear_interp);

	// rgb_backlight_update_keypress_effect_pixels();
	rgb_backlight_ripple_effect_update_pixels();
	combine_pixels(combined_pixels, &rgb_modes[rgb_mode_typing_react], &add);

	handle_on_off_animation(combined_pixels);
}

static void rgb_strip_float_2_rgb_strip(rgba_strip rgba, rgb_strip rgb) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		float alpha = rgba[i].a;

		rgb[i].r = CLAMP(rgba[i].r * 255, 0, 255) * alpha;
		rgb[i].g = CLAMP(rgba[i].g * 255, 0, 255) * alpha;
		rgb[i].b = CLAMP(rgba[i].b * 255, 0, 255) * alpha;
	}
}

static void apply_pixels(rgba_strip rgba_strip) {
	static rgb_strip strip;
	rgb_strip_float_2_rgb_strip(rgba_strip, strip);

	int err = led_strip_update_rgb(led_strip, strip, STRIP_NUM_PIXELS);
	if (err < 0) {
		// LOG_ERR("Failed to update the RGB strip (%d)", err);
	}
}

void rgb_backlight_tick(struct k_work *work) {
	set_pixels(combined_pixels);
	apply_pixels(combined_pixels);

	int index = 10;
	debug_set_text_fmt("%d %d %d %d", (int)(combined_pixels[index].r * 255),
					   (int)(combined_pixels[index].g * 255), (int)(combined_pixels[index].b * 255),
					   (int)(combined_pixels[index].a * 100));
	// FIX: hard coded index
	if (combined_pixels[index].a == 0.0) {
		debug_newline_text("backlight off");
		rgb_backlight_stop();
	}
}
