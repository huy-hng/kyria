#pragma once

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/drivers/led_strip.h>
#include <zmk/event_manager.h>

#include "headers/types.h"
#include "headers/control.h"
#include "headers/animations.h"
#include "behavior_defines.h"
#include "../imports.h"

extern const struct device *led_strip;
extern struct k_timer backlight_tick;
extern struct rgb_backlight_pixels rgb_pixels;
extern struct rgb_backlight_states rgb_states;

int rgb_backlight_save_state(int transition_ms);
int rgb_backlight_get_on_state(bool *state);

void rgb_backlight_tick(struct k_work *work);

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

void (*rgb_backlight_get_effect_fn(int effect))(rgb_strip_float pixels); // deprecated
