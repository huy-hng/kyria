#include "rgb_backlight.h"
#include "../utils.h"

//---------------------------------------------Getters----------------------------------------------

struct rgb_backlight_state *rgb_backlight_get_state() {
	return (struct rgb_backlight_state *)&rgb_state;
}

int rgb_backlight_get_on_state(bool *on_off) {
	if (!led_strip)
		return -ENODEV;

	*on_off = rgb_state.on;
	return 0;
}

//---------------------------------------------Effects----------------------------------------------

int rgb_backlight_calc_effect(int direction) {
	return (rgb_state.current_effect + RGB_BACKLIGHT_EFFECT_NUMBER + direction) % RGB_BACKLIGHT_EFFECT_NUMBER;
}

int rgb_backlight_select_effect(int effect) {
	if (!led_strip)
		return -ENODEV;

	if (effect < 0 || effect >= RGB_BACKLIGHT_EFFECT_NUMBER) {
		return -EINVAL;
	}

	rgb_state.current_effect = effect;
	// rgb_state.animation_step = 0;

	return rgb_backlight_save_state();
}

int rgb_backlight_cycle_effect(int direction) {
	return rgb_backlight_select_effect(rgb_backlight_calc_effect(direction));
}

//----------------------------------------------Change----------------------------------------------

int rgb_backlight_toggle() {
	return rgb_state.on ? rgb_backlight_off() : rgb_backlight_on();
}

int rgb_backlight_change_hue(int direction) {
	if (!led_strip)
		return -ENODEV;

	rgb_state.color = rgb_backlight_calc_hue(direction);

	return rgb_backlight_save_state();
}

int rgb_backlight_change_sat(int direction) {
	if (!led_strip)
		return -ENODEV;

	rgb_state.color = rgb_backlight_calc_sat(direction);

	return rgb_backlight_save_state();
}

int rgb_backlight_change_brt(int direction) {
	if (!led_strip)
		return -ENODEV;

	rgb_state.color = rgb_backlight_calc_brt(direction);

	return rgb_backlight_save_state();
}

int rgb_backlight_change_spd(int direction) {
	if (!led_strip)
		return -ENODEV;

	if (rgb_state.animation_speed == 1 && direction < 0) {
		return 0;
	}

	rgb_state.animation_speed += direction;

	if (rgb_state.animation_speed > 5) {
		rgb_state.animation_speed = 5;
	}

	return rgb_backlight_save_state();
}

//-----------------------------------------------Set------------------------------------------------

void rgb_backlight_set_peripheral_hsb(struct zmk_led_hsb color) {
	send_to_peripheral(RGB_UG, RGB_COLOR_HSB_CMD, RGB_COLOR_HSB_VAL(color.h, color.s, color.b));
}

int rgb_backlight_set_hue(int value) {
	if (!led_strip)
		return -ENODEV;

	rgb_state.color.h = value % HUE_MAX;
	rgb_backlight_set_peripheral_hsb(rgb_state.color);

	return rgb_backlight_save_state();
}

int rgb_backlight_set_sat(int value) {
	if (!led_strip)
		return -ENODEV;
	rgb_state.color.s = CLAMP(value, 0, SAT_MAX);
	rgb_backlight_set_peripheral_hsb(rgb_state.color);

	return rgb_backlight_save_state();
}

int rgb_backlight_set_brt(int value) {
	if (!led_strip)
		return -ENODEV;

	rgb_state.color.b = CLAMP(value, 0, BRT_MAX);
	rgb_backlight_set_peripheral_hsb(rgb_state.color);

	return rgb_backlight_save_state();
}

int rgb_backlight_set_spd(int value) {
	if (!led_strip)
		return -ENODEV;

	rgb_state.animation_speed = CLAMP(value, 1, 5);
	return rgb_backlight_save_state();
}

int rgb_backlight_set_hsb(struct zmk_led_hsb color) {
	if (color.h > HUE_MAX || color.s > SAT_MAX || color.b > BRT_MAX) {
		return -ENOTSUP;
	}

	rgb_state.color = color;

	return 0;
}

//----------------------------------------------Utils-----------------------------------------------

struct zmk_led_hsb rgb_backlight_calc_hue(int direction) {
	struct zmk_led_hsb color = rgb_state.color;

	color.h += HUE_MAX + (direction * CONFIG_ZMK_RGB_UNDERGLOW_HUE_STEP);
	color.h %= HUE_MAX;

	return color;
}

struct zmk_led_hsb rgb_backlight_calc_sat(int direction) {
	struct zmk_led_hsb color = rgb_state.color;

	int s = color.s + (direction * CONFIG_ZMK_RGB_UNDERGLOW_SAT_STEP);
	if (s < 0) {
		s = 0;
	} else if (s > SAT_MAX) {
		s = SAT_MAX;
	}
	color.s = s;

	return color;
}

struct zmk_led_hsb rgb_backlight_calc_brt(int direction) {
	struct zmk_led_hsb color = rgb_state.color;

	int b = color.b + (direction * CONFIG_ZMK_RGB_UNDERGLOW_BRT_STEP);
	color.b = CLAMP(b, 0, BRT_MAX);

	return color;
}

