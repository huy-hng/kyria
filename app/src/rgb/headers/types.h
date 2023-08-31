#pragma once

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/drivers/led_strip.h>
#include <zmk/event_manager.h>

#define STRIP_CHOSEN DT_CHOSEN(zmk_underglow)
#define STRIP_NUM_PIXELS DT_PROP(STRIP_CHOSEN, chain_length)
#define OVERGLOW_INDEX_START 6
#define UNDERGLOW_INDEX_START 0
#define UNDERGLOW_NUM_PIXELS 6

#define HUE_MAX 360
#define SAT_MAX 100
#define BRT_MAX 100

// TODO: put turn off duration somewhere else
#define TURN_OFF_DURATION 1000
// #define CONFIG_RGB_REFRESH_MS 10
// #define CONFIG_RGB_TRANSITION_DURATION 200

typedef struct {
	float h;
	float s;
	float b;
} led_hsbf;

struct led_hsb {
	uint16_t h;
	uint8_t s;
	uint8_t b;
};

struct led_rgb_float {
	float r;
	float g;
	float b;
};

typedef struct led_hsb hsb_strip[STRIP_NUM_PIXELS];
typedef struct led_rgb rgb_strip[STRIP_NUM_PIXELS];
typedef struct led_rgb_float rgb_strip_float[STRIP_NUM_PIXELS];

struct rgb_backlight_pixels {
	rgb_strip_float active;
	rgb_strip_float base;
	rgb_strip_float underglow;
	rgb_strip_float transition_start;
	rgb_strip_float transition_end;
	rgb_strip_float layer_color;
	rgb_strip_float keypress;
};

struct rgb_backlight_state {
	struct led_hsb color;
	uint8_t animation_speed;
	uint8_t current_effect;
	uint16_t animation_step;
	bool on;

	uint16_t transition_steps_left;
};

struct rgb_backlight_states {
	struct rgb_backlight_state *active;
	struct rgb_backlight_state base;
	struct rgb_backlight_state underglow;
	struct rgb_backlight_state layer_color;
};
