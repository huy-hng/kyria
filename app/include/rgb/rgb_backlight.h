#pragma once

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/drivers/led_strip.h>
#include <zmk/event_manager.h>

#include "types.h"
#include "control.h"
#include "utils.h"
#include "animations.h"
#include "behavior_defines.h"
// #include "../../imports.h"
#include "imports.h"

extern const struct device *led_strip;
extern struct rgb_backlight_pixels rgb_pixels;
extern struct rgb_backlight_states rgb_states;

int rgb_backlight_save_state(int transition_ms);
int rgb_backlight_get_on_state(bool *state);

void rgb_backlight_tick(struct k_work *work);