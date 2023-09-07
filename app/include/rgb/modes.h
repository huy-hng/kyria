#pragma once

#include "rgb/types.h"

enum rgb_mode_index {
	rgb_mode_base,
	rgb_mode_underglow,
	rgb_mode_layer_color,
	rgb_mode_key_react,
	rgb_mode_number, // keep track of number of modes
};

void rgb_backlight_initialize_modes();
