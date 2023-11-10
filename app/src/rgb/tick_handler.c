#include "rgb/rgb_backlight.h"

rgba_strip combined_pixels;

static void combine_pixels(struct rgb_backlight_mode *mode) {
	blending_fn *blend_fn = *mode->blend_fn;
	float alpha = mode->color.a / 100.0;
	for (int i = mode->range.start; i < mode->range.end; i++) {
		combined_pixels[i].r = blend_fn(combined_pixels[i].r, mode->pixels[i].r, alpha);
		combined_pixels[i].g = blend_fn(combined_pixels[i].g, mode->pixels[i].g, alpha);
		combined_pixels[i].b = blend_fn(combined_pixels[i].b, mode->pixels[i].b, alpha);
	}
}

static bool handle_on_off_animation() {
	float step_size = ((float)CONFIG_RGB_REFRESH_MS / TURN_OFF_DURATION);

	if (!rgb_state.on)
		step_size *= -1; // invert to subtract step_size

	bool is_off = true;
	// FIX: unnecessary to run each tick, only on on/off state change
	//      add an event handler or similar
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		combined_pixels[i].a = CLAMP(combined_pixels[i].a + step_size, 0.0, 1.0);
		if (combined_pixels[i].a > 0) {
			is_off = false;
		}
	}
	return is_off;
}

static void rgb_strip_float_2_rgb_strip(rgba_strip rgba, rgb_strip rgb) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		float alpha = rgba[i].a;

		rgb[i].r = CLAMP(rgba[i].r * 255, 0, 255) * alpha;
		rgb[i].g = CLAMP(rgba[i].g * 255, 0, 255) * alpha;
		rgb[i].b = CLAMP(rgba[i].b * 255, 0, 255) * alpha;
	}
}

static void update_led_strip() {
	static rgb_strip strip;
	rgb_strip_float_2_rgb_strip(combined_pixels, strip);
	int err = led_strip_update_rgb(led_strip, strip, STRIP_NUM_PIXELS);
	if (err < 0) {
		// LOG_ERR("Failed to update the RGB strip (%d)", err);
	}
}

void rgb_backlight_tick(struct k_work *work) {
	for (int i = 0; i < rgb_mode_number; i++) {
		if (!rgb_modes[i].enabled)
			continue;

		rgb_modes[i].set_pixels(&rgb_modes[i]);
		combine_pixels(&rgb_modes[i]);
	}
	bool is_off = handle_on_off_animation();

	update_led_strip();

	if (is_off)
		rgb_backlight_stop();
}
