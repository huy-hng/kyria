#include <zmk/keymap.h>
#include <zmk/hid.h>
#include <zmk/events/keycode_state_changed.h>

#include "rgb/rgb_backlight.h"
#include "display/widgets/debug_output.h"

typedef struct {
	int index;
	struct led_hsb color;
	int effect;
} LayerColor;

static struct rgb_backlight_state base_state;
static int prev_index;

// clang-format off
struct led_hsb white  = {          .s = 1, /* .b = 0.6f */};
struct led_hsb desat  = {          .s = 60,/* .b = 0.8f */};
struct led_hsb red    = {.h =   1,         /* .b = 0.8f */};
struct led_hsb orange = {.h =  18,         /* .b = 0.8f */};
struct led_hsb yellow = {.h =  48,                  };
struct led_hsb green  = {.h = 120,         /* .b = 0.8f */};
struct led_hsb cyan   = {.h = 142,         /* .b = 0.8f */};
struct led_hsb blue   = {.h = 210,         /* .b = 0.8f */};
struct led_hsb indigo = {.h = 256,                  };
struct led_hsb pink   = {.h = 300,                  };

static LayerColor layer_colors[15];

int layer_color_init() {
	LayerColor layers[] = {
		{ .index = BASE, .effect = 3  },
		{ .index = NAVIPAD,    indigo },
		{ .index = VIM,        indigo },
		{ .index = SYMBOLS,    cyan   },
		{ .index = MEDIA_FN,   green  },
		{ .index = OS,         blue   },
		{ .index = ENC_LR,     orange },
		// { .index = LAYER_MENU, white },
	};
	// clang-format on

	for (int i = 0; i < sizeof(layers) / sizeof(layers[0]); i++) {
		layer_colors[i] = layers[i];
	}
	return 0;
}

LayerColor *get_layer_color(int layer_index) {
	for (int i = 0; i < sizeof(layer_colors) / sizeof(layer_colors[0]); i++) {
		if (layer_index == layer_colors[i].index) {
			return &layer_colors[i];
		}
	}
	return NULL;
}

void set_color(struct led_hsb color) {
	if (!color.h)
		color.h = base_state.color.h;
	if (!color.s)
		color.s = base_state.color.s;
	if (!color.b) {
		color.b = base_state.color.b;
	} else if (color.b > 0 && color.b < 1) {
		color.b = (uint8_t)base_state.color.b * color.b;
	}
	invoke_behavior_global(RGB_UG, RGB_SET_UDG_HSB_CMD,
						   RGB_COLOR_HSB_VAL((int)color.h, (int)color.s, (int)color.b));
}

void rgb_backlight_update_layer_color() {
	uint8_t index = zmk_keymap_highest_layer_active();
	if (prev_index == BASE || prev_index == SETTINGS || prev_index == DISPLAY_MENU)
		base_state = rgb_states.base;

	prev_index = index;

	LayerColor *layer = get_layer_color(index);
	if (!layer)
		return;

	// int behavior_cmd = RGB_EFS_CMD;
	int behavior_cmd = RGB_EFS_UDG;
	if (index == BASE) {
		invoke_behavior_global("RGB_UG", behavior_cmd, RGB_UNDERGLOW_ANIMATION_COPY);
		set_color((struct led_hsb){});
		return;
	}

	set_color(layer->color);
	invoke_behavior_global("RGB_UG", behavior_cmd, RGB_UNDERGLOW_ANIMATION_SOLID);
}

int layer_color_event_listener(const zmk_event_t *eh) {
	if (same_str(eh->event->name, "zmk_layer_state_changed")) {
		rgb_backlight_update_layer_color();
		return 0;
	}

	// NOTE: theres no distinction between modifier color change and layer color change
	// they will therefore mix and behave unexpectedly
	// also key release has to be handled more gracefully
	const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
	if (ev) {
		uint8_t mods = zmk_hid_get_explicit_mods();
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
