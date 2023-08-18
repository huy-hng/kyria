#include "rgb_extra.c"

int zmk_rgb_underglow_set_hue(int value) {
	if (!led_strip)
		return -ENODEV;

	state.color.h = value % HUE_MAX;

	return zmk_rgb_underglow_save_state();
}

int zmk_rgb_underglow_set_sat(int value) {
	if (!led_strip)
		return -ENODEV;
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

struct rgb_underglow_state_extra *zmk_rgb_underglow_return_state() {
	return (struct rgb_underglow_state_extra *)&state;
}
