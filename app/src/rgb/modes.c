#include "rgb/rgb_backlight.h"

void rgb_backlight_initialize_modes() {
	// clang-format off
	rgb_modes[rgb_mode_base] = (struct rgb_backlight_mode){
		.color = {
			.h = CONFIG_ZMK_RGB_UNDERGLOW_HUE_START,
			.s = CONFIG_ZMK_RGB_UNDERGLOW_SAT_START,
			.b = CONFIG_ZMK_RGB_UNDERGLOW_BRT_START,
			.a = 100,
		},
		.active_animation = CONFIG_ZMK_RGB_UNDERGLOW_EFF_START,
		.animation_speed = CONFIG_ZMK_RGB_UNDERGLOW_SPD_START,
		.animation_step = 0,
		.on = IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_ON_START),

		.range = pixel_range.overglow,
		.blend_mode = pixel_blend_mode_replace,
	};
	// clang-format on

	rgb_modes[rgb_mode_underglow] = (struct rgb_backlight_mode){
		.active_animation = RGB_UNDERGLOW_ANIMATION_COPY,
		.range = pixel_range.underglow,
		.color.a = 100,
		.blend_mode = pixel_blend_mode_replace,
	};

	rgb_modes[rgb_mode_layer_color] = (struct rgb_backlight_mode){
		.active_animation = RGB_BACKLIGHT_ANIMATION_SOLID,
		// .range = pixel_range.underglow,
		.range = pixel_range.strip,
		.color.b = 20,
		.color.a = 0,
		.blend_mode = pixel_blend_mode_average,
	};

	rgb_modes[rgb_mode_key_react] = (struct rgb_backlight_mode){
		.active_animation = RGB_BACKLIGHT_ANIMATION_OFF,
		.range = pixel_range.overglow,
		.color.a = 0,
		.blend_mode = pixel_blend_mode_add,
	};

	// rgb_states.transition = rgb_states.base;
	// rgb_states.transition.alpha = 0;
}
