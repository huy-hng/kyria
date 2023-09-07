#include "rgb/types.h"

enum RGB_MODE_INDEX {
	RGB_MODE_BASE,
	RGB_MODE_UNDERGLOW,
	RGB_MODE_LAYER_COLOR,
	RGB_MODE_KEY_REACT,
};

struct rgb_backlight_states {
	struct rgb_backlight_state base;
	struct rgb_backlight_state underglow;
	struct rgb_backlight_state layer_color;
	struct rgb_backlight_state key_react;
	// struct rgb_backlight_state transition;
};

void rgb_backlight_initialize_states();
struct rgb_backlight_state *rgb_backlight_get_state_by_index(enum RGB_MODE_INDEX index);
