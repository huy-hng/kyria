#include <zmk/events/position_state_changed.h>
#include "rgb/rgb_backlight.h"
#include "imports.h"

#define KEYPRESS_EFFECT_ACTIVATION_TIME 100
#define KEYPRESS_EFFECT_DECAY_TIME 200
#define KEYPRESS_EFFECT_BRIGHTNESS_FACTOR 0.5
#define MAX_RIPPLES 10
#define NUM_PIXELS 25

const float PROPAGATION_SPEED = 8.0 / CONFIG_RGB_REFRESH_MS;
const float TRANSITION_SIZE = 2;

typedef struct {
	bool pressed;
	long timestamp;
	float outer_radius;
	float inner_radius;
} keypress_state;

struct ripple_data {
	uint8_t index;
	bool pressed;
	float outer_radius;
	float inner_radius;
	bool running;
	float pixels[NUM_PIXELS];
	float distance_mask[NUM_PIXELS];
	float max_radius;
};

static keypress_state states[NUM_PIXELS];
static struct ripple_data ongoing_animations[MAX_RIPPLES];

float max_radius;

// clang-format off
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
static int pixel_indices[NUM_PIXELS] = {
			25, 26, 27, 28, 29, 30,
			19, 20, 21, 22, 23, 24,
	11, 12, 13, 14, 15, 16, 17, 18,
	 6,  7,  8,  9, 10
};

static float coordinates[NUM_PIXELS][2] = {
	                         {11  , 0.4}, {12  , 0.3}, {13  , 0},   {14, 0.3}, {15, 0.9}, {16, 0.9},
	                         {11  , 1.4}, {12  , 1.3}, {13  , 1},   {14, 1.3}, {15, 1.9}, {16, 1.9},
	{8.6, 3.9}, { 9.9, 3  }, {11  , 2.4}, {12  , 2.3}, {13  , 2},   {14, 2.3}, {15, 2.9}, {16, 2.9},
	{9.3, 4.6}, {10.3, 3.9}, {11.4, 3.5}, {12.6, 3.3}, {13.6, 3.3}
};
#else
static int pixel_indices[NUM_PIXELS] = {
	30, 29, 28, 27, 26, 25,
	24, 23, 22, 21, 20, 19,
	18, 17, 16, 15, 14, 13, 12, 11,
				10,  9,  8,  7,  6,
};
static float coordinates[NUM_PIXELS][2] = {
	{0, 0.9}, {1, 0.9}, {2, 0.3}, {3  , 0   }, {4  , 0.3}, {5  , 0.4},
	{0, 1.9}, {1, 1.9}, {2, 1.3}, {3  , 1   }, {4  , 1.3}, {5  , 1.4},
	{0, 2.9}, {1, 2.9}, {2, 2.3}, {3  , 2   }, {4  , 2.3}, {5  , 2.4}, {6.1, 3  }, {7.4, 3.9},
                                  {2.4, 3.3 }, {3.4, 3.3}, {4.6, 3.5}, {5.7, 3.9}, {6.7, 4.6},
};

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
#endif
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
		struct ripple_data data = ongoing_animations[i];
		if (!data.running)
			continue;

		counter++;
		debug_set_text_fmt("%d index", data.index);
		debug_newline_text_fmt("%d pressed", data.pressed);
		debug_newline_text_fmt("%d running", data.running);
		debug_newline_text_fmt("%s outer", ftos(data.outer_radius, 2));
		debug_newline_text_fmt("%s inner", ftos(data.inner_radius, 2));
	}
	debug_newline_text_fmt("%d", counter);
}

static int convert_position_to_index(int pos) {
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

static void add_ripple(struct ripple_data data) {
	for (int i = 0; i < MAX_RIPPLES; i++) {
		if (!ongoing_animations[i].running) {
			ongoing_animations[i] = data;
			return;
		}
	}
}

// returns pointer to `struct ripple_data` or NULL if key_index isn't found
static void *get_ripple(uint8_t key_index) {
	for (int i = 0; i < MAX_RIPPLES; i++) {
		if (ongoing_animations[i].index == key_index) {
			return &ongoing_animations[i];
		}
	}
	return NULL;
}

static void unpress_ripple(uint8_t key_index) {
	// loop through all ripples and unpress all of them, since a key can be in the list multiple
	// times
	for (int i = 0; i < MAX_RIPPLES; i++)
		if (ongoing_animations[i].index == key_index)
			ongoing_animations[i].pressed = false;
}

static void del_ripple(uint8_t key_index) {
	for (int i = 0; i < MAX_RIPPLES; i++) {
		if (ongoing_animations[i].index == key_index) {
			ongoing_animations[i].running = false;
			return;
		}
	}
}

static void update_radii(struct ripple_data *data) {
	data->outer_radius += PROPAGATION_SPEED;
	if (!data->pressed)
		data->inner_radius += PROPAGATION_SPEED;

	if (data->inner_radius > data->max_radius * 1.5)
		data->running = false;
}

static void update_surrounding_pixels(struct ripple_data *data) {
	float max_brt = (rgb_modes[rgb_mode_base].color.b * KEYPRESS_EFFECT_BRIGHTNESS_FACTOR) / 100.0;
	float activation = (max_brt * CONFIG_RGB_REFRESH_MS) / 200;

	struct rgb_backlight_mode *mode = &rgb_modes[rgb_mode_typing_react];
	if (!data->running)
		return;

	float ripple_layer[NUM_PIXELS];
	for (int i = 0; i < NUM_PIXELS; i++) {
		float radius;
		float brt_ratio;
		float brightness = 0;
		// TODO: dry it up
		if (data->distance_mask[i] < data->inner_radius) {
			radius = data->inner_radius;
			brt_ratio = (radius - data->distance_mask[i]) / TRANSITION_SIZE;
			brightness = CLAMP(1 - brt_ratio, 0, 1) * max_brt;
			// set_pixel_white(&mode->pixels[pixel_indices[i]], brightness, 1.0);

		} else if (data->distance_mask[i] < data->outer_radius) {
			radius = data->outer_radius;
			brt_ratio = (radius - data->distance_mask[i]) / TRANSITION_SIZE;
			brightness = CLAMP(brt_ratio, 0, 1) * max_brt;
			// set_pixel_white(&mode->pixels[pixel_indices[i]], brightness, 1.0);
		}
		if (brightness != 0)
			set_pixel_white(&mode->pixels[pixel_indices[i]], brightness, 1.0);
		// ripple_layer[i] = brightness;
		// 	change_pixel_white(&mode->pixels[pixel_indices[i]], brightness, 1.0);
	}
	// for (int i = 0; i < NUM_PIXELS; i++) {
	// 	float brightness = ripple_layer[i];
	// 	change_pixel_white(&mode->pixels[pixel_indices[i]], brightness, 1.0);
	// }
	update_radii(data);
}

void rgb_backlight_ripple_effect_update_pixels() {
	struct rgb_backlight_mode *mode = &rgb_modes[rgb_mode_typing_react];

	print_ripple_data();
	for (int i = 0; i < MAX_RIPPLES; i++) {
		update_surrounding_pixels(&ongoing_animations[i]);
	}

	// float max_brt = keypress_effect_calc_max_brightness();
	// float decay = -(max_brt * CONFIG_RGB_REFRESH_MS) / 1000;
	return;

	for (int i = 0; i < NUM_PIXELS; i++) {
		// line below should be outside of loop; did this cause weird behavior?
		struct rgb_backlight_mode *mode = &rgb_modes[rgb_mode_typing_react];
		// update_surrounding_pixels(i);

		// if (!states[i].pressed) {
		// 	states[i].outer_radius = 0;
		// 	// change_keypress_pixel(&mode->pixels[pixel_indices[i]], decay);
		// 	set_pixel_white(&mode->pixels[pixel_indices[i]], 0);

		// 	continue;

		// 	// states[i].radius_active = CLAMP(states[i].radius_active + 0.1, 0, max_radius);
		// }
		// float max_brt = keypress_effect_calc_max_brightness();
		// float activation = (max_brt * CONFIG_RGB_REFRESH_MS) / 200;

		// set_pixel_white(&mode->pixels[pixel_indices[i]], 0.5);
		// // change_keypress_pixel(&mode->pixels[pixel_indices[i]], activation);

		// // update_surrounding_pixels(i);
		// states[i].outer_radius = CLAMP(states[i].outer_radius + 0.1, 0, max_radius);
	}
	// debug_set_text_fmt("%d %d %d", (int)(states[DEBUG_KEY_INDEX].outer_radius * 10),
	// 				   (int)(states[DEBUG_KEY_INDEX].inner_radius * 10), (int)(max_radius * 10));
}

void rgb_backlight_ripple_effect_event_handler(const zmk_event_t *eh) {
	struct zmk_position_state_changed *pos = as_zmk_position_state_changed(eh);
	if (pos->source == 0)
		return;

	int index = convert_position_to_index(pos->position);

	// TEST: what happens if pos->state is false but ripple data doesnt exist
	//       if event handler wasnt fired due to quick keypresses
	// FIX: feels a little verbose to check for state,
	//      then if not state get data and set its state to false
	if (pos->state) {
		struct ripple_data data = {.index = index, .pressed = pos->state, .running = true};
		generate_distance_mask(&data);
		add_ripple(data);
	} else
		unpress_ripple(index);
}
