#include <zmk/events/position_state_changed.h>
#include "rgb/rgb_backlight.h"
#include "imports.h"

#define KEYPRESS_EFFECT_ACTIVATION_TIME 100
#define KEYPRESS_EFFECT_DECAY_TIME 1000
#define KEYPRESS_EFFECT_BRIGHTNESS_FACTOR 2

static int keypress_lightup_state[STRIP_NUM_PIXELS];

// clang-format off
static int position_to_pixel[50] = {
	30, 29, 28, 27, 26, 25,         		25, 26, 27, 28, 29, 30,
	24, 23, 22, 21, 20, 19,         		19, 20, 21, 22, 23, 24,
	18, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 18,
				10,  9,  8,  7,  6,  6,  7,  8,  9, 10
};
// clang-format on

float keypress_effect_calc_max_brightness() {
	return (rgb_states.base.color.b * KEYPRESS_EFFECT_BRIGHTNESS_FACTOR) / 100.0;
}

void change_keypress_pixel(struct led_rgba *pixel, float value) {
	float max_brt = keypress_effect_calc_max_brightness();

	pixel->r = CLAMP(pixel->r + value, 0, max_brt);
	pixel->g = CLAMP(pixel->g + value, 0, max_brt);
	pixel->b = CLAMP(pixel->b + value, 0, max_brt);
}

void rgb_backlight_update_keypress_effect_pixels() {
	float max_brt = keypress_effect_calc_max_brightness();
	float KEYPRESS_EFFECT_DECAY_SPEED =
		-(max_brt * CONFIG_RGB_REFRESH_MS) / KEYPRESS_EFFECT_DECAY_TIME;
	float KEYPRESS_EFFECT_ACTIVATION_SPEED =
		(max_brt * CONFIG_RGB_REFRESH_MS) / KEYPRESS_EFFECT_ACTIVATION_TIME;

	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		float value = keypress_lightup_state[i] ? KEYPRESS_EFFECT_ACTIVATION_SPEED
												: KEYPRESS_EFFECT_DECAY_SPEED;
		change_keypress_pixel(&rgb_pixels.keypress[i], value);
	}
}

void rgb_backlight_keypress_lightup_event_handler(const zmk_event_t *eh) {
	struct zmk_position_state_changed *pos = as_zmk_position_state_changed(eh);
	int pix_index = position_to_pixel[pos->position];
	if (pos->source > 0)
		keypress_lightup_state[pix_index] = pos->state;
}
