#pragma once

#include "types.h"

struct led_hsb hsb_scale_min_max(struct led_hsb hsb);
struct led_hsb hsb_scale_zero_max(struct led_hsb hsb);
struct led_rgba hsb_to_rgb(struct led_hsb hsb);
void copy_pixel_array(rgba_strip arr1, rgba_strip arr2);
void set_pixel(struct led_rgba *pixel, float r, float g, float b, bool absolute);
void set_pixel_white(struct led_rgba *pixel, float value, bool absolute);
void rgb_strip_float_2_rgb_strip(rgba_strip rgb_float, rgb_strip rgb);

void (*rgb_backlight_get_effect_fn(int effect))(rgba_strip pixels); // deprecated
