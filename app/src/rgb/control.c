#include "rgb/rgb_backlight.h"

int rgb_backlight_on() {
	if (!led_strip)
		return -ENODEV;

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
	send_to_peripheral(RGB_UG, RGB_ON_CMD, 0);
#endif
	return rgb_backlight_start();
}

int rgb_backlight_off() {
	if (!led_strip)
		return -ENODEV;

	rgb_state.on = false;

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
	// send_to_peripheral(RGB_UG, RGB_OFF_CMD, 0);
#endif
	return rgb_backlight_save_state(-1);
}

int rgb_backlight_get_on_state(bool *on_off) {
	if (!led_strip)
		return -ENODEV;

	*on_off = rgb_state.on;
	return 0;
}

//---------------------------------------------Effects----------------------------------------------

int rgb_backlight_calc_effect(int direction) {
	return (rgb_modes[rgb_mode_base].active_animation + RGB_BACKLIGHT_EFFECT_NUMBER + direction) %
		   RGB_BACKLIGHT_EFFECT_NUMBER;
}

int rgb_backlight_select_effect(int effect, struct rgb_backlight_mode *state) {
	if (!led_strip)
		return -ENODEV;

	if (state == NULL)
		state = &rgb_modes[rgb_mode_base];

	// if (effect < 0 || effect >= RGB_BACKLIGHT_EFFECT_NUMBER)
	// 	return -EINVAL;

	state->active_animation = effect;

	return rgb_backlight_save_state(-1);
}

int rgb_backlight_cycle_effect(int direction) {
	return rgb_backlight_select_effect(rgb_backlight_calc_effect(direction), &rgb_modes[rgb_mode_base]);
}

//----------------------------------------------Change----------------------------------------------

int rgb_backlight_toggle() { return rgb_state.on ? rgb_backlight_off() : rgb_backlight_on(); }

int rgb_backlight_change_hue(int direction) {
	if (!led_strip)
		return -ENODEV;

	rgb_modes[rgb_mode_base].color = rgb_backlight_calc_hue(direction);

	return rgb_backlight_save_state(-1);
}

int rgb_backlight_change_sat(int direction) {
	if (!led_strip)
		return -ENODEV;

	rgb_modes[rgb_mode_base].color = rgb_backlight_calc_sat(direction);

	return rgb_backlight_save_state(-1);
}

int rgb_backlight_change_brt(int direction) {
	if (!led_strip)
		return -ENODEV;

	rgb_modes[rgb_mode_base].color = rgb_backlight_calc_brt(direction);

	return rgb_backlight_save_state(-1);
}

int rgb_backlight_change_spd(int direction) {
	if (!led_strip)
		return -ENODEV;

	if (rgb_modes[rgb_mode_base].animation_speed == 1 && direction < 0) {
		return 0;
	}

	rgb_modes[rgb_mode_base].animation_speed += direction;

	if (rgb_modes[rgb_mode_base].animation_speed > 5) {
		rgb_modes[rgb_mode_base].animation_speed = 5;
	}

	return rgb_backlight_save_state(0);
}

//-----------------------------------------------Set------------------------------------------------

void rgb_backlight_set_peripheral_hsb(struct led_hsb color) {
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
	send_to_peripheral(RGB_UG, RGB_COLOR_HSB_CMD, RGB_COLOR_HSB_VAL(color.h, color.s, color.b));
#endif
}

void rgb_backlight_set_peripheral_behavior(int param1, int param2) {
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
	send_to_peripheral(RGB_UG, param1, param2);
#endif
}

int rgb_backlight_set_hue(int value) {
	if (!led_strip)
		return -ENODEV;

	rgb_modes[rgb_mode_base].color.h = value % HUE_MAX;
	rgb_backlight_set_peripheral_behavior(RGB_SET_HUE, rgb_modes[rgb_mode_base].color.h);

	return rgb_backlight_save_state(-1);
}

int rgb_backlight_set_sat(int value) {
	if (!led_strip)
		return -ENODEV;
	rgb_modes[rgb_mode_base].color.s = CLAMP(value, 0, SAT_MAX);
	rgb_backlight_set_peripheral_behavior(RGB_SET_SAT, rgb_modes[rgb_mode_base].color.s);

	return rgb_backlight_save_state(-1);
}

int rgb_backlight_set_brt(int value) {
	if (!led_strip)
		return -ENODEV;

	rgb_modes[rgb_mode_base].color.b = CLAMP(value, 0, BRT_MAX);
	rgb_backlight_set_peripheral_behavior(RGB_SET_BRT, rgb_modes[rgb_mode_base].color.b);

	return rgb_backlight_save_state(-1);
}

int rgb_backlight_set_spd(int value) {
	if (!led_strip)
		return -ENODEV;

	rgb_modes[rgb_mode_base].animation_speed = CLAMP(value, 1, 5);
	return rgb_backlight_save_state(0);
}

int rgb_backlight_set_hsb(struct led_hsb color, struct rgb_backlight_mode *state) {
	if (state == NULL)
		state = &rgb_modes[rgb_mode_base];

	if (color.h > HUE_MAX || color.s > SAT_MAX || color.b > BRT_MAX)
		return -ENOTSUP;

	state->color = color;
	return rgb_backlight_save_state(-1);
}

//----------------------------------------------Utils-----------------------------------------------

struct led_hsb rgb_backlight_calc_hue(int direction) {
	struct led_hsb color = rgb_modes[rgb_mode_base].color;

	color.h += HUE_MAX + (direction * CONFIG_ZMK_RGB_UNDERGLOW_HUE_STEP);
	color.h %= HUE_MAX;

	return color;
}

struct led_hsb rgb_backlight_calc_sat(int direction) {
	struct led_hsb color = rgb_modes[rgb_mode_base].color;

	int s = color.s + (direction * CONFIG_ZMK_RGB_UNDERGLOW_SAT_STEP);
	if (s < 0) {
		s = 0;
	} else if (s > SAT_MAX) {
		s = SAT_MAX;
	}
	color.s = s;

	return color;
}

struct led_hsb rgb_backlight_calc_brt(int direction) {
	struct led_hsb color = rgb_modes[rgb_mode_base].color;

	int b = color.b + (direction * CONFIG_ZMK_RGB_UNDERGLOW_BRT_STEP);
	color.b = CLAMP(b, 0, BRT_MAX);

	return color;
}
