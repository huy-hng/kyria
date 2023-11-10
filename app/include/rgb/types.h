#pragma once

#include <zephyr/drivers/led_strip.h>

#define STRIP_CHOSEN DT_CHOSEN(zmk_underglow)
#define STRIP_NUM_PIXELS DT_PROP(STRIP_CHOSEN, chain_length)

#define HUE_MAX 360
#define SAT_MAX 100
#define BRT_MAX 100

// TODO: put turn off duration somewhere else
#define TURN_OFF_DURATION 500
// #define CONFIG_RGB_REFRESH_MS 500
// #define CONFIG_RGB_TRANSITION_DURATION 200

struct start_end {
	uint8_t start;
	uint8_t end;
};

extern struct rgb_backlight_pixel_range {
	struct start_end strip; // all
	struct start_end overglow;
	struct start_end underglow;
} pixel_range;

struct led_hsb {
	uint16_t h; // 0 - 360
	uint8_t s;	// 0 - 100
	uint8_t b;	// 0 - 100
	float a;	// i think range is between 0 and 100  TODO: needs checking
};

struct led_rgba {
	// everything is between 0 - 1 (i think)
	float r;
	float g;
	float b;
	float a;
};

typedef struct led_hsb hsb_strip[STRIP_NUM_PIXELS];
typedef struct led_rgb rgb_strip[STRIP_NUM_PIXELS];
typedef struct led_rgba rgba_strip[STRIP_NUM_PIXELS];

enum {
	rgb_mode_base,
	rgb_mode_underglow,
	rgb_mode_typing_react,
	rgb_mode_key_layer,
	rgb_mode_number, // keep track of number of modes
};

typedef float blending_fn(float, float, float);
extern struct rgb_backlight_blending_fn {
	blending_fn *add;
	blending_fn *mask;
	blending_fn *replace;
	blending_fn *weighted_interp;
	blending_fn *linear_interp;
} blending_fns;

struct rgb_backlight_mode;
typedef void set_pixels_fn(struct rgb_backlight_mode *);

extern struct rgb_backlight_mode {
	bool on;
	bool enabled;
	struct start_end range;
	struct led_hsb color;

	uint8_t active_animation;
	uint8_t animation_speed;
	uint16_t animation_step;

	set_pixels_fn *set_pixels;

	blending_fn *blend_fn;
	rgba_strip pixels;

} rgb_modes[];

// enum ColorSpace {
// 	RGB,
// 	RGBA,

// 	HSB,
// 	HSBA,

// 	HSL,
// 	HSLA
// };

// typedef struct {
// 	enum ColorSpace current_type;

// 	struct led_rgba rgba;
// 	struct led_hsb hsb;
// 	// struct led_hsba hsba;
// } Pixel;

// struct Pixels {
// 	enum ColorSpace active_color_space;
// 	float alpha;
// 	hsb_strip pixels_hsb;
// 	rgba_strip pixels_rgba;
// };
