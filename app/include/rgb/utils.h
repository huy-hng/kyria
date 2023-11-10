#pragma once

#include "types.h"

struct led_hsb hsb_scale_min_max(struct led_hsb hsb);
struct led_hsb hsb_scale_zero_max(struct led_hsb hsb);
struct led_rgba hsb_to_rgb(struct led_hsb hsb);

struct led_rgba create_rgba_pixel(float r, float g, float b, float alpha);

void set_pixel(struct led_rgba *pixel, float r, float g, float b, float a);
void set_pixel_white(struct led_rgba *pixel, float value, float alpha);

void change_pixel(struct led_rgba *pixel, float r, float g, float b, float a);
void change_pixel_white(struct led_rgba *pixel, float value, float alpha);

void (*rgb_backlight_get_effect_fn(int effect))(rgba_strip pixels); // deprecated
