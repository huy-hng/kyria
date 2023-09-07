#include "rgb/rgb_backlight.h"

struct rgb_backlight_state *get_state_by_index(enum RGB_MODE_INDEX index) {
	switch (index) {
	case RGB_MODE_BASE:
		return &rgb_states.base;
	case RGB_MODE_UNDERGLOW:
		return &rgb_states.underglow;
	case RGB_MODE_LAYER_COLOR:
		return &rgb_states.layer_color;
	case RGB_MODE_KEY_REACT:
		return &rgb_states.key_react;
	}

	return NULL;
}

void rgb_backlight_initialize_states() {
	// clang-format off
	rgb_states.base = (struct rgb_backlight_state){
		.color = {
			.h = CONFIG_ZMK_RGB_UNDERGLOW_HUE_START,
			.s = CONFIG_ZMK_RGB_UNDERGLOW_SAT_START,
			.b = CONFIG_ZMK_RGB_UNDERGLOW_BRT_START,
			// .a = 100,
		},
		.active_animation = CONFIG_ZMK_RGB_UNDERGLOW_EFF_START,
		.animation_speed = CONFIG_ZMK_RGB_UNDERGLOW_SPD_START,
		.animation_step = 0,
		.on = IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_ON_START),
		.range = pixel_range.overglow
	};
	// clang-format on

	rgb_states.underglow = rgb_states.base;
	rgb_states.underglow.active_animation = RGB_UNDERGLOW_ANIMATION_COPY;
	rgb_states.underglow.range = pixel_range.underglow;
	// rgb_states.underglow.color.a = 100;

	rgb_states.layer_color = rgb_states.base;
	rgb_states.layer_color.active_animation = RGB_BACKLIGHT_ANIMATION_SOLID;
	rgb_states.layer_color.range = pixel_range.underglow;
	// rgb_states.underglow.color.a = 0;

	rgb_states.key_react = rgb_states.base;
	rgb_states.key_react.active_animation = RGB_BACKLIGHT_ANIMATION_OFF;
	// rgb_states.underglow.color.a = 0;

	// rgb_states.transition = rgb_states.base;
	// rgb_states.transition.alpha = 0;
}
