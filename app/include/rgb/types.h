#pragma once

#include <zephyr/drivers/led_strip.h>

#define STRIP_CHOSEN DT_CHOSEN(zmk_underglow)
#define STRIP_NUM_PIXELS DT_PROP(STRIP_CHOSEN, chain_length)

#define HUE_MAX 360
#define SAT_MAX 100
#define BRT_MAX 100

// TODO: put turn off duration somewhere else
#define TURN_OFF_DURATION 1000
// #define CONFIG_RGB_REFRESH_MS 500
// #define CONFIG_RGB_TRANSITION_DURATION 200

struct start_end {
	uint8_t start;
	uint8_t end;
};

struct rgb_backlight_pixel_range {
	struct start_end strip; // all
	struct start_end overglow;
	struct start_end underglow;
};

struct led_hsb {
	uint16_t h; // 0 - 360
	uint8_t s; // 0 - 100
	uint8_t b; // 0 - 100
	float a; // i think range is between 0 and 100  TODO: needs checking
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

enum pixel_blend_mode {
	pixel_blend_mode_replace,
	pixel_blend_mode_average,
	pixel_blend_mode_add,
	pixel_blend_mode_subtract,
};


struct rgb_backlight_mode {
	bool on;
	struct start_end range;
	struct led_hsb color;

	uint8_t active_animation;
	uint8_t animation_speed;
	uint16_t animation_step;

	enum pixel_blend_mode blend_mode;
	rgba_strip pixels;

	// uint16_t transition_steps_left;
};

enum ColorSpace {
	RGB,
	RGBA,

	HSB,
	HSBA,

	HSL,
	HSLA
};

typedef struct {
	enum ColorSpace current_type;

	struct led_rgba rgba;
	struct led_hsb hsb;
	// struct led_hsba hsba;
} Pixel;

struct Pixels {
	enum ColorSpace active_color_space;
	float alpha;
	hsb_strip pixels_hsb;
	rgba_strip pixels_rgba;
};
