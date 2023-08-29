#include <zmk/keymap.h>
#include <zmk/hid.h>
#include <zmk/events/keycode_state_changed.h>

#include "rgb_backlight.h"
#include "../imports.h"
#include "../display/widgets/headers/debug_output.h"

struct color {
	uint16_t h;
	uint8_t s;
	float b;
};

struct layer_color {
	char *label;
	struct color color;
	int effect;
};

struct rgb_backlight_state base_state;
static int prev_index;

// clang-format off
struct color white  = {          .s = 1, /* .b = 0.6f */};
struct color desat  = {          .s = 60,/* .b = 0.8f */};
struct color red    = {.h =   1,         /* .b = 0.8f */};
struct color orange = {.h =  18,         /* .b = 0.8f */};
struct color yellow = {.h =  48,                  };
struct color green  = {.h = 120,         /* .b = 0.8f */};
struct color cyan   = {.h = 142,         /* .b = 0.8f */};
struct color blue   = {.h = 210,         /* .b = 0.8f */};
struct color indigo = {.h = 256,                  };
struct color pink   = {.h = 300,                  };
struct layer_color layer_colors[20];

int layer_color_init() {
    layer_colors[0] = (struct layer_color){.label = "Colemak", .effect = 3};
    layer_colors[1] = (struct layer_color){.label = "Navipad",  indigo};
    layer_colors[2] = (struct layer_color){.label = "Vim",      indigo};
    layer_colors[3] = (struct layer_color){.label = "Symbols",  cyan};
    layer_colors[4] = (struct layer_color){.label = "Layers",   white};
    layer_colors[5] = (struct layer_color){.label = "Media FN", green};
    layer_colors[6] = (struct layer_color){.label = "OS",       blue};
    layer_colors[7] = (struct layer_color){.label = "Enc LR",   orange};
	// clang-format on
	return 0;
}

struct layer_color *get_layer_color(const char *layer_label) {
	for (int i = 0; i < sizeof(layer_colors) / sizeof(layer_colors[0]); i++) {
		if (same_str(layer_label, layer_colors[i].label)) {
			return &layer_colors[i];
		}
	}
	return NULL;
}

void set_color(struct color color) {
	if (!color.h)
		color.h = base_state.color.h;
	if (!color.s)
		color.s = base_state.color.s;
	if (!color.b) {
		color.b = base_state.color.b;
	} else if (color.b > 0 && color.b < 1) {
		color.b = (uint8_t)base_state.color.b * color.b;
	}

	invoke_behavior_global(RGB_UG, RGB_COLOR_HSB_CMD, RGB_COLOR_HSB_VAL(color.h, color.s, color.b));
}

void update_layer_color() {
	uint8_t index = zmk_keymap_highest_layer_active();
	const char *label = zmk_keymap_layer_label(index);
	if (prev_index == BASE || prev_index == SETTINGS || prev_index == DISPLAY_MENU)
		base_state = *rgb_backlight_get_state();

	prev_index = index;

	struct layer_color *layer = get_layer_color(label);
	if (!layer)
		return;

	struct rgb_backlight_state *state = rgb_backlight_get_state();
	if (index == BASE) {
		// state->underglow_effect = RGB_UNDERGLOW_EFFECT_COPY;
		invoke_behavior_global("RGB_UG", RGB_EFS_CMD, base_state.current_effect);
		set_color((struct color){});
		return;
	}

	set_color(layer->color);
	// state->underglow_effect = RGB_UNDERGLOW_EFFECT_SOLID;
	invoke_behavior_global("RGB_UG", RGB_EFS_CMD, 0);
}

int layer_color_event_listener(const zmk_event_t *eh) {
	if (same_str(eh->event->name, "zmk_layer_state_changed")) {
		update_layer_color();
		return 0;
	}

	// NOTE: theres no distinction between modifier color change and layer color change
	// they will therefore mix and behave unexpectedly
	// also key release has to be handled more gracefully

	const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
	if (ev) {
		uint8_t mods = zmk_hid_get_explicit_mods();
		// struct rgb_underglow_state state = *rgb_backlight_return_state();
		if (ev->state && mods > 0) {
			invoke_behavior_global(RGB_UG, RGB_SET_SAT, 60);
			// invoke_behavior_global(RGB_SET_BRT, state.color.b * 0.8);
		} else if (!ev->state && mods == 0) {
			invoke_behavior_global(RGB_UG, RGB_SET_SAT, 100);
			// invoke_behavior_global(RGB_SET_SAT, base_state.color.s);
			// invoke_behavior_global(RGB_SET_BRT, base_state.color.b);
		}
	}

	return 0;
}
