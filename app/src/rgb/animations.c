#include "rgb/rgb_backlight.h"

struct led_rgba to_rgb(struct led_hsb hsb) { return hsb_to_rgb(hsb_scale_min_max(hsb)); }

struct led_rgba get_off_pixel(struct rgb_backlight_mode *mode, uint8_t i) {
	return create_rgba_pixel(0, 0, 0, 1);
}

struct led_rgba get_solid_pixel(struct rgb_backlight_mode *mode, uint8_t i) {
	return to_rgb(mode->color);
}

struct led_rgba get_breathe_pixel(struct rgb_backlight_mode *mode, uint8_t i) {
	struct led_hsb hsb = mode->color;
	hsb.b = abs(mode->animation_step - 1200) / 12;
	return to_rgb(hsb);
}

struct led_rgba get_spectrum_pixel(struct rgb_backlight_mode *mode, uint8_t i) {
	struct led_hsb hsb = mode->color;
	hsb.h = mode->animation_step;
	return to_rgb(hsb);
}

struct led_rgba get_swirl_pixel(struct rgb_backlight_mode *mode, uint8_t i) {
	struct led_hsb hsb = mode->color;
	int range = mode->range.end - mode->range.start;
	hsb.h = (HUE_MAX / range * i + mode->animation_step) % HUE_MAX;

	return to_rgb(hsb);
}

struct led_rgba get_sparkle_pixel(struct rgb_backlight_mode *mode, uint8_t i) {
	struct led_hsb hsb = mode->color;
	int hue_offset = i * 69691;
	hsb.h = (hue_offset + mode->animation_step) % HUE_MAX;

	return to_rgb(hsb);
}

//-----------------------------------------underglow----------------------------------------

// first value is num of pixels because i dont know how to code
int i_0[] = {1, 25};
int i_1[] = {2, 26, 27};
int i_2[] = {1, 29};
int i_3[] = {2, 10, 17};
int i_4[] = {2, 8, 9};
int i_5[] = {2, 6, 11};
int *right_underglow_lookup[6] = {i_0 + 1, i_1 + 1, i_2 + 1, i_3 + 1, i_4 + 1, i_5 + 1};

static struct led_rgba average_overglow(struct rgb_backlight_mode *state, uint8_t i) {
	int *indices = right_underglow_lookup[i];
	int arr_len = indices[-1];

	float r = 0, g = 0, b = 0;

	// struct rgb_backlight_mode *overglow = &rgb_modes[rgb_mode_base];

	for (int i = 0; i < arr_len; i++) {
		// pixel = overglow->pixels[indices[i]];
		struct led_rgba pixel = combined_pixels[indices[i]];
		r += SQUARE(pixel.r);
		g += SQUARE(pixel.g);
		b += SQUARE(pixel.b);
	}

	return (struct led_rgba){
		.r = sqrtf(r / arr_len),
		.g = sqrtf(g / arr_len),
		.b = sqrtf(b / arr_len),
		.a = 1,
	};
}

void set_mode_pixels(struct rgb_backlight_mode *state, animation_fn fn) {
	for (int i = state->range.start; i < state->range.end; i++)
		state->pixels[i] = fn(state, i);
}

void rgb_backlight_set_animation_pixels(struct rgb_backlight_mode *mode) {
	int animation_step_size = mode->animation_speed;
	int modulo = HUE_MAX;

	switch (mode->active_animation) {
	case RGB_BACKLIGHT_ANIMATION_OFF:
		return set_mode_pixels(mode, &get_off_pixel);
	case RGB_BACKLIGHT_ANIMATION_SOLID:
		return set_mode_pixels(mode, &get_solid_pixel);
	case RGB_UNDERGLOW_ANIMATION_COPY:
		return set_mode_pixels(mode, &average_overglow);

	case RGB_BACKLIGHT_ANIMATION_BREATHE:
		set_mode_pixels(mode, &get_breathe_pixel);
		animation_step_size = mode->animation_speed * 5;
		modulo = 2400;
		break;
	case RGB_BACKLIGHT_ANIMATION_SPECTRUM:
		set_mode_pixels(mode, &get_spectrum_pixel);
		break;
	case RGB_BACKLIGHT_ANIMATION_SWIRL:
		set_mode_pixels(mode, &get_swirl_pixel);
		break;
	case RGB_BACKLIGHT_ANIMATION_SPARKLE:
		set_mode_pixels(mode, &get_sparkle_pixel);
		break;
	}

	mode->animation_step += animation_step_size;
	mode->animation_step %= modulo;
}
