#include <zmk/events/position_state_changed.h>
#include "../rgb_backlight.h"
#include "../../imports.h"

#define KEYPRESS_EFFECT_ACTIVATION_TIME 100
#define KEYPRESS_EFFECT_DECAY_TIME 1000
// #define KEYPRESS_EFFECT_BRIGHTNESS 0.3
static int keypress_lightup_state[STRIP_NUM_PIXELS];

// clang-format off
int position_pixels[50] = {
	30, 29, 28, 27, 26, 25,         		25, 26, 27, 28, 29, 30,
	24, 23, 22, 21, 20, 19,         		19, 20, 21, 22, 23, 24,
	18, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 18,
				10,  9,  8,  7,  6,  6,  7,  8,  9, 10
};
// clang-format on

void set_keypress_pixel(struct led_rgb_float *pixel, float value) {
	float max_brt = (rgb_states.base.color.b * 2) / 100.0;

	pixel->r = CLAMP(pixel->r + value, 0, max_brt);
	pixel->g = CLAMP(pixel->g + value, 0, max_brt);
	pixel->b = CLAMP(pixel->b + value, 0, max_brt);
}

void rgb_backlight_update_keypress_effect_pixels() {
	float max_brt = (rgb_states.base.color.b * 3) / 100.0;
	float KEYPRESS_EFFECT_DECAY_SPEED =
		-(max_brt * CONFIG_RGB_REFRESH_MS) / KEYPRESS_EFFECT_DECAY_TIME;
	float KEYPRESS_EFFECT_ACTIVATION_SPEED =
		(max_brt * CONFIG_RGB_REFRESH_MS) / KEYPRESS_EFFECT_ACTIVATION_TIME;

	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		float value = keypress_lightup_state[i] ? KEYPRESS_EFFECT_ACTIVATION_SPEED
												: KEYPRESS_EFFECT_DECAY_SPEED;
		set_keypress_pixel(&rgb_pixels.keypress[i], value);
	}
}

void rgb_backlight_keypress_lightup_event_handler(const zmk_event_t *eh) {
	struct zmk_position_state_changed *pos = as_zmk_position_state_changed(eh);
	int pix_index = position_pixels[pos->position];

	if (pos->source > 0) {
		// debug_newline_text_fmt("%d", pix_index);
		keypress_lightup_state[pix_index] = pos->state;
	}
}
