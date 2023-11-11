#include <zmk/events/position_state_changed.h>
#include "rgb/rgb_backlight.h"
#include "imports.h"

// TODO: instead of updating all ripples pixels, update one pixel of each ripple and combine their
// brightness, this removes the necessity for pixels member in `ripple_data`, which in turns saves
// on memory (and possibly calculation?)

#define BRIGHTNESS_FACTOR 0.2
#define PROPAGATION_SPEED 10.0 / CONFIG_RGB_REFRESH_MS
#define TRANSITION_SIZE 4.0

#define MAX_RIPPLES 10
#define NUM_PIXELS 25

struct ripple_data {
	uint8_t index;
	bool pressed;
	bool is_running;
	float outer_radius;
	float inner_radius;
	float max_radius;

	float pixels[NUM_PIXELS];
	float distance_mask[NUM_PIXELS];
};

static struct ripple_data ripples[MAX_RIPPLES];

// clang-format off
static int array_index_to_pixel_index[NUM_PIXELS] = {
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
			25, 26, 27, 28, 29, 30,
			19, 20, 21, 22, 23, 24,
	11, 12, 13, 14, 15, 16, 17, 18,
	 6,  7,  8,  9, 10
#else
	30, 29, 28, 27, 26, 25,
	24, 23, 22, 21, 20, 19,
	18, 17, 16, 15, 14, 13, 12, 11,
				10,  9,  8,  7,  6
#endif
};

static float coordinates[NUM_PIXELS][2] = {
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
	                         {11  , 0.4}, {12  , 0.3}, {13  , 0},   {14, 0.3}, {15, 0.9}, {16, 0.9},
	                         {11  , 1.4}, {12  , 1.3}, {13  , 1},   {14, 1.3}, {15, 1.9}, {16, 1.9},
	{8.6, 3.9}, { 9.9, 3  }, {11  , 2.4}, {12  , 2.3}, {13  , 2},   {14, 2.3}, {15, 2.9}, {16, 2.9},
	{9.3, 4.6}, {10.3, 3.9}, {11.4, 3.5}, {12.6, 3.3}, {13.6, 3.3}
#else
	{0, 0.9}, {1, 0.9}, {2, 0.3}, {3  , 0  }, {4  , 0.3}, {5  , 0.4},
	{0, 1.9}, {1, 1.9}, {2, 1.3}, {3  , 1  }, {4  , 1.3}, {5  , 1.4},
	{0, 2.9}, {1, 2.9}, {2, 2.3}, {3  , 2  }, {4  , 2.3}, {5  , 2.4}, {6.1, 3  }, {7.4, 3.9},
	                             {2.4, 3.3}, {3.4, 3.3}, {4.6, 3.5}, {5.7, 3.9}, {6.7, 4.6},
#endif
};
// clang-format on

static void print_mask(float *mask) {
	debug_set_text("");
	for (int i = 0; i < NUM_PIXELS; i++) {
		int val = mask[i] * 10;

		if (i > 0)
			debug_newline_text(" ");

		debug_append_text_fmt("%d", val);

		if (i % 6 == 0 && i > 0)
			debug_newline_text("");

		if (val < 10)
			debug_append_text(" ");

		if (i % 6 == 0)
			debug_append_text_fmt("%d", val);
		else
			debug_append_text_fmt(" %d", val);
	}
}

static void print_ripple_data() {
	debug_set_text("");
	int counter = 0;
	for (int i = 0; i < MAX_RIPPLES; i++) {
		struct ripple_data ripple = ripples[i];
		if (!ripple.is_running)
			continue;

		// if (i == 0)
		// 	debug_set_text("");

		counter++;
		debug_newline_text("");
		if (ripple.index < 10)
			debug_append_text(" ");
		debug_append_text_fmt("%d %d", ripple.index, ripple.pressed);
		debug_append_text_fmt(" %s", ftos(ripple.inner_radius, 1));
		debug_append_text_fmt(" %s", ftos(ripple.outer_radius, 1));
		debug_append_text_fmt(" %s", ftos(ripple.max_radius, 1));
	}
	if (counter > 0)
		debug_newline_text_fmt("%d", counter);
}

static int get_position_to_array_index(int pos) {
	// relative indices (per side)
	//  0  1  2  3  4  5               0  1  2  3  4  5
	//  6  7  8  9 10 11               6  7  8  9 10 11
	// 12 13 14 15 16 17 18 19  12 13 14 15 16 17 18 19
	// 	        20 21 22 23 24  20 21 22 23 24

	// absolute indices
	//  0  1  2  3  4  5               6  7  8  9 10 11
	// 12 13 14 15 16 17              18 19 20 21 22 23
	// 24 25 26 27 28 29 30 31  32 33 34 35 36 37 38 39
	// 	        40 41 42 43 44  45 46 47 48 49
	int offset = 0;

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

static float calc_key_distance(float *p1, float *p2) {
	return sqrt(SQUARE(p2[0] - p1[0]) + SQUARE(p2[1] - p1[1]));
}

static void generate_distance_mask(struct ripple_data *data) {
	float *start = coordinates[data->index];
	for (int i = 0; i < NUM_PIXELS; i++) {
		float distance = calc_key_distance(start, coordinates[i]);
		data->distance_mask[i] = distance;
		data->max_radius = MAX(data->max_radius, distance);
	}
}

static void update_radii(struct ripple_data *data) {
	data->outer_radius += PROPAGATION_SPEED;
	if (!data->pressed)
		data->inner_radius += PROPAGATION_SPEED;

	if (data->inner_radius > data->max_radius + TRANSITION_SIZE)
		data->is_running = false;
}

static void add_ripple(struct ripple_data data) {
	for (int i = 0; i < MAX_RIPPLES; i++) {
		if (!ripples[i].is_running) {
			ripples[i] = data;
			return;
		}
	}
}

static void unpress_ripple(uint8_t key_index) {
	// loop through all ripples and unpress all of them, since a key can be in the list multiple
	// times
	for (int i = 0; i < MAX_RIPPLES; i++)
		if (ripples[i].index == key_index)
			ripples[i].pressed = false;
}

static void update_surrounding_pixels(struct ripple_data *ripple) {
	if (!ripple->is_running)
		return;

	float max_brt = (rgb_modes[rgb_mode_base].color.b * BRIGHTNESS_FACTOR) / 100.0;
	for (int i = 0; i < NUM_PIXELS; i++) {
		float brightness = 0;

		float distance_from_center = ripple->distance_mask[i];
		if (ripple->outer_radius > distance_from_center) {
			float brt_ratio = (ripple->outer_radius - distance_from_center) / TRANSITION_SIZE;
			brightness = CLAMP(brt_ratio, 0, 1) * max_brt;
		}

		if (ripple->inner_radius > distance_from_center) {
			float brt_ratio = (ripple->inner_radius - distance_from_center) / TRANSITION_SIZE;
			float inner_brt = CLAMP(1 - brt_ratio, 0, 1) * max_brt;
			brightness = MIN(brightness, inner_brt);
		}

		ripple->pixels[i] = brightness;
	}
	update_radii(ripple);
}

void rgb_backlight_ripple_effect_update_pixels() {
	for (int i = 0; i < MAX_RIPPLES; i++)
		update_surrounding_pixels(&ripples[i]);

	struct rgb_backlight_mode *mode = &rgb_modes[rgb_mode_typing_react];

	for (int i = 0; i < NUM_PIXELS; i++) {
		float combined_brt = 0;
		for (int j = 0; j < MAX_RIPPLES; j++)
			if (ripples[j].is_running)
				combined_brt += ripples[j].pixels[i];

		set_pixel_white(&mode->pixels[array_index_to_pixel_index[i]], combined_brt, 1.0);
	}
}

void rgb_backlight_ripple_effect_event_handler(const zmk_event_t *eh) {
	struct zmk_position_state_changed *pos = as_zmk_position_state_changed(eh);
	if (pos->source == 0)
		return;

	int index = get_position_to_array_index(pos->position);

	// FIX: feels a little verbose to check for state,
	//      then if not state get data and set its state to false
	if (pos->state) {
		struct ripple_data data = {.index = index, .pressed = pos->state, .is_running = true};
		generate_distance_mask(&data);
		add_ripple(data);
	} else
		unpress_ripple(index);
}
