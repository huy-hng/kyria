#include "rgb_underglow.c"
#include "rgb_extra.h"

enum rgb_underglow_effect_extra {
	UNDERGLOW_EFFECT_SPARKLE = 4,
};

static void zmk_rgb_underglow_effect_sparkle() {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		struct zmk_led_hsb hsb = state.color;

		int hue_offset = i * 69691;
		hsb.h = (hue_offset + state.animation_step) % HUE_MAX;

		pixels[i] = hsb_to_rgb(hsb_scale_min_max(hsb));
	}

	state.animation_step += state.animation_speed;
	state.animation_step = state.animation_step % HUE_MAX;
}

void zmk_rgb_underglow_tick_extra(struct k_work *work) {
	switch (state.current_effect) {
	case UNDERGLOW_EFFECT_SOLID:
		zmk_rgb_underglow_effect_solid();
		break;
	case UNDERGLOW_EFFECT_BREATHE:
		zmk_rgb_underglow_effect_breathe();
		break;
	case UNDERGLOW_EFFECT_SPECTRUM:
		zmk_rgb_underglow_effect_spectrum();
		break;
	case UNDERGLOW_EFFECT_SWIRL:
		zmk_rgb_underglow_effect_swirl();
		break;
	case UNDERGLOW_EFFECT_SPARKLE:
		zmk_rgb_underglow_effect_sparkle();
		break;
	}

	int err = led_strip_update_rgb(led_strip, pixels, STRIP_NUM_PIXELS);
	if (err < 0) {
		LOG_ERR("Failed to update the RGB strip (%d)", err);
	}
}

int zmk_rgb_underglow_set_hue(int value) {
	if (!led_strip)
		return -ENODEV;

	state.color.h = value % HUE_MAX;

	return zmk_rgb_underglow_save_state();
}

int zmk_rgb_underglow_set_sat(int value) {
	if (!led_strip)
		return -ENODEV;
	LOG_DBG("set sat %d", value);
	state.color.s = CLAMP(value, 0, SAT_MAX);

	return zmk_rgb_underglow_save_state();
}

int zmk_rgb_underglow_set_brt(int value) {
	if (!led_strip)
		return -ENODEV;

	state.color.b = CLAMP(value, 0, BRT_MAX);

	return zmk_rgb_underglow_save_state();
}

int zmk_rgb_underglow_set_spd(int value) {
	if (!led_strip)
		return -ENODEV;

	state.animation_speed = CLAMP(value, 1, 5);
	return zmk_rgb_underglow_save_state();
}

struct rgb_underglow_state_extra zmk_rgb_underglow_return_state() {
	struct rgb_underglow_state_extra *s = (struct rgb_underglow_state_extra *)&state;
	return *s;
	// return state;
}
