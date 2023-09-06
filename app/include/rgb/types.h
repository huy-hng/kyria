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
	struct start_end strip;
	struct start_end overglow;
	struct start_end underglow;
};

struct led_hsba {
	float h;
	float s;
	float b;
	float a;
};
// } led_hsbf;

struct led_hsb {
	uint16_t h;
	uint8_t s;
	uint8_t b;
};

struct led_rgba {
	float r;
	float g;
	float b;
	float a;
};

typedef struct led_hsb hsb_strip[STRIP_NUM_PIXELS];
typedef struct led_rgb rgb_strip[STRIP_NUM_PIXELS];
typedef struct led_rgba rgba_strip[STRIP_NUM_PIXELS];

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
	struct led_hsba hsba;
} Pixel;

struct Pixels {
	enum ColorSpace active_color_space;
	float alpha;
	hsb_strip pixels_hsb;
	rgba_strip pixels_rgba;
};

struct rgb_backlight_pixels {
	rgba_strip active;
	rgba_strip base;
	rgba_strip underglow;
	rgba_strip transition_start;
	rgba_strip transition_end;
	rgba_strip layer_color;
	rgba_strip keypress;
};

struct rgb_backlight_state {
	bool on;
	struct start_end range;
	struct led_hsb color;
	float alpha;

	uint8_t active_animation;
	uint8_t animation_speed;
	uint16_t animation_step;

	// uint16_t transition_steps_left;
	rgba_strip pixels;

};

struct rgb_backlight_states {
	struct rgb_backlight_state base;
	struct rgb_backlight_state underglow;
	struct rgb_backlight_state layer_color;
	struct rgb_backlight_state key_react;
	// struct rgb_backlight_state transition;
};
