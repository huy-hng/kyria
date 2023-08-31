#pragma once

#include "types.h"

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
