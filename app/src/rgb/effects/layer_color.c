#include "rgb/rgb_backlight.h"
#include "display/widgets/debug_output.h"
#include "zephyr/sys/util.h"

#define MAX_OPACITY 50.0f

typedef struct {
	float h;
	float s;
	float b;
	// float a;
} hsbf;

// clang-format off
#define WHITE  (hsbf) {  -1,  0, -1 } // 0.6
#define DESAT  (hsbf) {  -1, 60, -1 } // 0.8
#define RED    (hsbf) {   1, -1, -1 } // 0.8
#define ORANGE (hsbf) {  18, -1, -1 } // 0.8
#define YELLOW (hsbf) {  48, -1, -1 } //  -1
#define GREEN  (hsbf) { 120, -1, -1 } // 0.8
#define CYAN   (hsbf) { 142, -1, -1 } // 0.8
#define BLUE   (hsbf) { 210, -1, -1 } // 0.8
#define INDIGO (hsbf) { 256, -1, -1 } //  -1
#define PINK   (hsbf) { 300, -1, -1 } //  -1

typedef struct {
	uint8_t index;
	hsbf color;
	uint8_t effect;
} LayerColor;

static LayerColor layer_colors[] = {
	{ .index = BASE,              },
	{ .index = DEBUG_SCREEN,      },
	{ .index = NAVIPAD,    INDIGO },
	{ .index = VIM,        INDIGO },
	{ .index = SYMBOLS,    CYAN   },
	{ .index = MEDIA_FN,   GREEN  },
	{ .index = OS,         BLUE   },
	{ .index = ENC_LR,     ORANGE },
	{ .index = LAYER_MENU, WHITE },
};
// clang-format on

hsbf *get_layer_color(uint8_t layer_index) {
	for (int i = 0; i < ARRAY_SIZE(layer_colors); i++) {
		if (layer_index == layer_colors[i].index) {
			return &layer_colors[i].color;
		}
	}
	return NULL;
}

void rgb_backlight_set_layer_color(uint8_t active_layer_index) {
	hsbf *layer_color = get_layer_color(active_layer_index);
	if (!layer_color)
		return;

	struct led_hsb *color = &rgb_modes[rgb_mode_key_layer].color;
	float step_size = MAX_OPACITY / ((float)CONFIG_RGB_TRANSITION_DURATION / CONFIG_RGB_REFRESH_MS);

	if (active_layer_index == BASE || active_layer_index == DEBUG_SCREEN) {
		// what does this do?
		color->a = MAX(color->a - step_size, 0);
		return;
	}

	struct led_hsb base_color = rgb_modes[rgb_mode_base].color;
	color->h = base_color.h;
	color->s = base_color.s;
	color->b = base_color.b;

	if (layer_color->h >= 0)
		color->h = layer_color->h;
	if (layer_color->s >= 0)
		color->s = layer_color->s;
	if (layer_color->b >= 0)
		color->b = layer_color->b;
	// else if (color.b > 0 && color.b < 1)
	// 	color.b = (uint8_t)base_state.color.b * color.b;

	// color->b = base_color.b * 1.3;

	float alpha = MIN(color->a + step_size, MAX_OPACITY);
	color->a = alpha;

	// rgb_modes[rgb_mode_layer_color].range = pixel_range.overglow;
	// rgb_backlight_animation_solid(&rgb_modes[rgb_mode_layer_color]);
	rgb_backlight_animation_solid(&rgb_modes[rgb_mode_key_layer]);
}
