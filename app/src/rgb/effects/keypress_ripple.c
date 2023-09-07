#include <zmk/events/position_state_changed.h>
#include "rgb/rgb_backlight.h"
#include "imports.h"

typedef struct {
	bool pressed;
	long timestamp;
	float radius_active;
	float radius_inactive;
} keypress_state;

static keypress_state states[25];

float max_radius;
static float mask[25];

// clang-format off
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
static int pixel_indices[25] = {
			25, 26, 27, 28, 29, 30,
			19, 20, 21, 22, 23, 24,
	11, 12, 13, 14, 15, 16, 17, 18,
	 6,  7,  8,  9, 10
};

static float positions[25][2] = {
	                         {11  , 0.4}, {12  , 0.3}, {13  , 0},   {14, 0.3}, {15, 0.9}, {16, 0.9},
	                         {11  , 1.4}, {12  , 1.3}, {13  , 1},   {14, 1.3}, {15, 1.9}, {16, 1.9},
	{8.6, 3.9}, { 9.9, 3  }, {11  , 2.4}, {12  , 2.3}, {13  , 2},   {14, 2.3}, {15, 2.9}, {16, 2.9},
	{9.3, 4.6}, {10.3, 3.9}, {11.4, 3.5}, {12.6, 3.3}, {13.6, 3.3}
};


//  0  1  2  3  4  5               0  1  2  3  4  5
//  6  7  8  9 10 11               6  7  8  9 10 11
// 12 13 14 15 16 17 18 19  12 13 14 15 16 17 18 19
// 	        20 21 22 23 24  20 21 22 23 24

//  0  1  2  3  4  5               6  7  8  9 10 11
// 12 13 14 15 16 17              18 19 20 21 22 23
// 24 25 26 27 28 29 30 31  32 33 34 35 36 37 38 39
// 	        40 41 42 43 44  45 46 47 48 49
#else
static int pixel_indices[25] = {
	30, 29, 28, 27, 26, 25,
	24, 23, 22, 21, 20, 19,
	18, 17, 16, 15, 14, 13, 12, 11,
				10,  9,  8,  7,  6,
};
static float positions[25][2] = {
	{0, 0.9}, {1, 0.9}, {2, 0.3}, {3  , 0   }, {4  , 0.3}, {5  , 0.4},
	{0, 1.9}, {1, 1.9}, {2, 1.3}, {3  , 1   }, {4  , 1.3}, {5  , 1.4},
	{0, 2.9}, {1, 2.9}, {2, 2.3}, {3  , 2   }, {4  , 2.3}, {5  , 2.4}, {6.1, 3  }, {7.4, 3.9},
                                  {2.4, 3.3 }, {3.4, 3.3}, {4.6, 3.5}, {5.7, 3.9}, {6.7, 4.6},
};
#endif

int convert_position_to_index(int pos){
	int offset;

	if (pos < 6)
		offset = 0;
	else if (pos < 18)
		offset = 6;
	else if (pos < 32)
		offset = 12;
	else if (pos < 45)
		offset = 20;
	else
		offset = 25;

	return pos - offset;
}

// clang-format on

float calc_key_distance(float *p1, float *p2) {
	return sqrt(SQUARE(p2[0] - p1[0]) + SQUARE(p2[1] - p1[1]));
}

void distance_mask(int start_pos, float *mask, float *max_distance) {
	float *start = positions[start_pos];
	for (int i = 0; i < 25; i++) {
		float distance = calc_key_distance(start, positions[i]);
		mask[i] = distance;
		*max_distance = MAX(*max_distance, distance);
	}
}

void update_surrounding_pixels(int center) {
	float max_brt = keypress_effect_calc_max_brightness();
	float activation = (max_brt * CONFIG_RGB_REFRESH_MS) / 1000;

	for (int i = 0; i < 25; i++) {
		if (mask[i] > states[center].radius_active)
			continue;
		// float value = mask[i] <= states[center].radius_active ? activation : decay;

		change_keypress_pixel(&rgb_modes[rgb_mode_key_react].pixels[pixel_indices[i]], activation);
	}
}

void rgb_backlight_update_ripple_effect_pixels() {
	float max_brt = keypress_effect_calc_max_brightness();
	float decay = -(max_brt * CONFIG_RGB_REFRESH_MS) / 1000;
	for (int i = 0; i < 25; i++) {
		if (!states[i].pressed) {
			states[i].radius_active = 0;
			change_keypress_pixel(&rgb_modes[rgb_mode_key_react].pixels[pixel_indices[i]], decay);
			continue;
		}
		update_surrounding_pixels(i);
		states[i].radius_active = CLAMP(states[i].radius_active + 0.1, 0, max_radius);
		// float speed = states[i].pressed ? 0.1 : -0.1;
	}
}

void print_mask(float *mask) {
	debug_set_text("");
	for (int i = 0; i < 25; i++) {
		int val = mask[i] * 10;

		// if (i > 0)
		// 	debug_newline_text(" ");

		// debug_append_text_fmt("%d", val);


		// if (i % 6 == 0 && i > 0)
		// 	debug_newline_text("");

		// if (val < 10)
		// 	debug_append_text(" ");

		// if (i % 6 == 0)
		// 	debug_append_text_fmt("%d", val);
		// else
		// 	debug_append_text_fmt(" %d", val);
	}
}

void rgb_backlight_keypress_ripple_event_handler(const zmk_event_t *eh) {
	struct zmk_position_state_changed *pos = as_zmk_position_state_changed(eh);
	if (pos->source == 0)
		return;

	int index = convert_position_to_index(pos->position);
	states[index].pressed = pos->state;
	states[index].radius_active = 0;
	states[index].radius_inactive = 0;
	distance_mask(index, mask, &max_radius);
	// print_mask(mask);
}
