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
#include "modes.h"
#include "behavior_rgb_backlight.h"
#include "imports.h"

extern const struct device *led_strip;
extern struct rgb_backlight_mode rgb_modes[];
extern struct rgb_backlight_pixel_range pixel_range;

int rgb_backlight_save_state(int transition_ms);
int rgb_backlight_get_on_state(bool *state);

void rgb_backlight_tick(struct k_work *work);
