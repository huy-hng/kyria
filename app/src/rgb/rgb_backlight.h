#pragma once

#include <zephyr/drivers/led_strip.h>
#include <zephyr/init.h>
#include <zmk/rgb_underglow.h>
#include <dt-bindings/zmk/rgb.h>

#define RGB_SET_HUE 15
#define RGB_SET_SAT 16
#define RGB_SET_BRT 17

#define HUE_MAX 360
#define SAT_MAX 100
#define BRT_MAX 100

#define ANIMATION_REFRESH 25
#define ANIMATION_DURATION 200

#define STRIP_CHOSEN DT_CHOSEN(zmk_underglow)
#define STRIP_NUM_PIXELS DT_PROP(STRIP_CHOSEN, chain_length)

enum rgb_backlight_effects {
	RGB_BACKLIGHT_EFFECT_SOLID,
	RGB_BACKLIGHT_EFFECT_BREATHE,
	RGB_BACKLIGHT_EFFECT_SPECTRUM,
	RGB_BACKLIGHT_EFFECT_SWIRL,
	RGB_BACKLIGHT_EFFECT_SPARKLE,
	RGB_BACKLIGHT_EFFECT_TEST,

	RGB_BACKLIGHT_EFFECT_NUMBER // Used to track number of underglow effects
};

struct rgb_backlight_state {
	struct zmk_led_hsb color;
	uint8_t animation_speed;
	uint8_t current_effect;
	uint16_t animation_step;
	bool on;

	struct zmk_led_hsb target_color;
	struct led_rgb initial_pixels[STRIP_NUM_PIXELS];
	struct led_rgb pixels[STRIP_NUM_PIXELS];
	int steps_left;
	int total_steps;
};

typedef struct led_rgb rgb_strip[STRIP_NUM_PIXELS];

extern const struct device *led_strip;
extern rgb_strip pixels;
extern struct rgb_backlight_state rgb_state;

int rgb_backlight_save_state();
int rgb_backlight_get_on_state(bool *state);
struct rgb_backlight_state *rgb_backlight_get_state();

void rgb_backlight_start_transition_animation();
void rgb_backlight_tick(struct k_work *work);
int rgb_backlight_set_hsb(struct zmk_led_hsb color);
int rgb_backlight_set_hue(int value);
int rgb_backlight_set_sat(int value);
int rgb_backlight_set_brt(int value);
int rgb_backlight_set_spd(int value);
int rgb_backlight_change_hue(int direction);
int rgb_backlight_change_sat(int direction);
int rgb_backlight_change_brt(int direction);
int rgb_backlight_change_spd(int direction);

int rgb_backlight_toggle();
int rgb_backlight_on();
int rgb_backlight_off();
int rgb_backlight_cycle_effect(int direction);
int rgb_backlight_calc_effect(int direction);
int rgb_backlight_select_effect(int effect);
struct zmk_led_hsb rgb_backlight_calc_hue(int direction);
struct zmk_led_hsb rgb_backlight_calc_sat(int direction);
struct zmk_led_hsb rgb_backlight_calc_brt(int direction);


//---------------------------------------------effects----------------------------------------------

void rgb_backlight_effect_solid(/* rgb_strip pixels */);
void rgb_backlight_effect_breathe(/* rgb_strip pixels */);
void rgb_backlight_effect_spectrum(/* rgb_strip pixels */);
void rgb_backlight_effect_swirl(/* rgb_strip pixels */);
void rgb_backlight_effect_sparkle(/* rgb_strip pixels */);
void rgb_backlight_effect_solid_rainbow(/* rgb_strip pixels */);
void rgb_backlight_effect_test(/* rgb_strip pixels */);
