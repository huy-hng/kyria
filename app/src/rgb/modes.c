#include "rgb/rgb_backlight.h"

struct rgb_state rgb_state;
struct rgb_backlight_mode rgb_modes[rgb_mode_number];
struct rgb_backlight_pixel_range pixel_range = {
	.underglow = {.start = 0, .end = 6},
	.overglow = {.start = 6, .end = STRIP_NUM_PIXELS},
	.strip = {.start = 0, .end = STRIP_NUM_PIXELS},
};

void rgb_backlight_initialize_modes() {
	// clang-format off

	rgb_modes[rgb_mode_base] = (struct rgb_backlight_mode){
		// what is the function of this? Turn off base mode and have everything else still use rgb?
		.enabled = true, 

		.range = pixel_range.overglow,
		.blend_fn = blending_fns.replace,
		.set_pixels = &rgb_backlight_set_animation_pixels,

		.active_animation = rgb_state.active_animation,
		.animation_speed = rgb_state.animation_speed,
		.animation_step = 0,

		.color = {
			.h = rgb_state.color.h,
			.s = rgb_state.color.s,
			.b = rgb_state.color.b,
			.a = 100,
		},
	};

	rgb_modes[rgb_mode_underglow] = (struct rgb_backlight_mode){
		.enabled = true,

		.set_pixels = &rgb_backlight_set_animation_pixels,
		.range = pixel_range.underglow,
		.blend_fn = blending_fns.replace,

		.active_animation = RGB_UNDERGLOW_ANIMATION_COPY,
		.color.a = 100,
	};

	rgb_modes[rgb_mode_key_layer] = (struct rgb_backlight_mode){
		.enabled = true,

		.set_pixels = &rgb_backlight_set_key_layer_pixels,
		.range = pixel_range.strip, // pixel_range.underglow
		.blend_fn = blending_fns.linear_interp,

		.color.b = 20,
		.color.a = 0,
	};

	rgb_modes[rgb_mode_typing_react] = (struct rgb_backlight_mode){
		.enabled = true,

		.set_pixels = &rgb_backlight_ripple_effect_update_pixels,
		.range = pixel_range.overglow,
		.blend_fn = blending_fns.add
	};
	// clang-format on
}
