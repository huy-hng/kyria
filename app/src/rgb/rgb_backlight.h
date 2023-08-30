#pragma once

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/drivers/led_strip.h>
#include <zmk/event_manager.h>

#include "behavior_defines.h"
#include "../imports.h"

// #define CONFIG_RGB_REFRESH_MS 10
// #define CONFIG_RGB_TRANSITION_DURATION 200

#define HUE_MAX 360
#define SAT_MAX 100
#define BRT_MAX 100

// TODO: put turn off duration somewhere else
#define TURN_OFF_DURATION 1000

#define STRIP_CHOSEN DT_CHOSEN(zmk_underglow)
#define STRIP_NUM_PIXELS DT_PROP(STRIP_CHOSEN, chain_length)
#define OVERGLOW_INDEX_START 6
#define UNDERGLOW_INDEX_START 0
#define UNDERGLOW_NUM_PIXELS 6

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

enum rgb_backlight_effects {
	// RGB_BACKLIGHT_ANIMATION_OFF,
	RGB_BACKLIGHT_ANIMATION_SOLID,
	RGB_BACKLIGHT_ANIMATION_BREATHE,
	RGB_BACKLIGHT_ANIMATION_SPECTRUM,
	RGB_BACKLIGHT_ANIMATION_SWIRL,
	RGB_BACKLIGHT_ANIMATION_SPARKLE,
	RGB_BACKLIGHT_ANIMATION_TEST,

	RGB_BACKLIGHT_EFFECT_NUMBER // Used to track number of effects
};

enum rgb_underglow_effects {
	RGB_UNDERGLOW_ANIMATION_OFF,
	RGB_UNDERGLOW_ANIMATION_COPY,
	RGB_UNDERGLOW_ANIMATION_SOLID,
	RGB_UNDERGLOW_ANIMATION_NUMBER // Used to track number of effects
};

extern const struct device *led_strip;
extern struct k_timer backlight_tick;


struct rgb_backlight_pixels {
	rgb_strip_float active;
	rgb_strip_float base;
	rgb_strip_float underglow;
	rgb_strip_float transition_start;
	rgb_strip_float transition_end;
	rgb_strip_float layer_color;
	rgb_strip_float keypress;
} extern rgb_pixels;

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
} extern rgb_states;

int rgb_backlight_save_state();
int rgb_backlight_get_on_state(bool *state);
struct rgb_backlight_state *rgb_backlight_get_state();

void rgb_backlight_start_transition_effect();
void rgb_backlight_tick(struct k_work *work);
int rgb_backlight_set_hsb(struct led_hsb color, struct rgb_backlight_state *state);
int rgb_backlight_set_hue(int value);
int rgb_backlight_set_sat(int value);
int rgb_backlight_set_brt(int value);
int rgb_backlight_set_spd(int value);
int rgb_backlight_change_hue(int direction);
int rgb_backlight_change_sat(int direction);
int rgb_backlight_change_brt(int direction);
int rgb_backlight_change_spd(int direction);

int rgb_backlight_toggle();
int rgb_backlight_start();
int rgb_backlight_stop();
int rgb_backlight_on();
int rgb_backlight_off();
int rgb_backlight_cycle_effect(int direction);
int rgb_backlight_calc_effect(int direction);
int rgb_backlight_select_effect(int effect, struct rgb_backlight_state *state);
int rgb_underglow_select_effect(int effect);
struct led_hsb rgb_backlight_calc_hue(int direction);
struct led_hsb rgb_backlight_calc_sat(int direction);
struct led_hsb rgb_backlight_calc_brt(int direction);

//--------------------------------------------Animations--------------------------------------------

// clang-format off
void rgb_underglow_animation_set_pixels(int effect, rgb_strip_float pixels);
void rgb_backlight_animation_set_pixels(int effect, rgb_strip_float pixels);
void rgb_backlight_effect_off             (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_solid        (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_breathe      (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_spectrum     (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_swirl        (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_sparkle      (rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_solid_rainbow(rgb_strip_float pixels, int start, int end);
void rgb_backlight_animation_test         (rgb_strip_float pixels, int start, int end);
// clang-format on

void rgb_backlight_transition_step();
void rgb_backlight_keypress_lightup_event_handler(const zmk_event_t *eh);
void rgb_backlight_update_keypress_effect_pixels();

void (*rgb_backlight_get_effect_fn(int effect))(rgb_strip_float pixels); // deprecated

int layer_color_init();
void rgb_backlight_update_layer_color();

//----------------------------------------------Utils-----------------------------------------------

struct led_hsb hsb_scale_min_max(struct led_hsb hsb);
struct led_hsb hsb_scale_zero_max(struct led_hsb hsb);
struct led_rgb_float hsb_to_rgb(struct led_hsb hsb);
void copy_pixel_array(rgb_strip_float arr1, rgb_strip_float arr2);
void set_pixel(struct led_rgb_float *pixel, float r, float g, float b, bool absolute);
void set_pixel_white(struct led_rgb_float *pixel, float value, bool absolute);
void rgb_strip_float_2_rgb_strip(rgb_strip_float rgb_float, rgb_strip rgb);
