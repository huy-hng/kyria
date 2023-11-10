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
#include "behavior_rgb_backlight.h"
#include "imports.h"

extern const struct device *led_strip;
extern uint8_t active_layer_index;
extern rgba_strip combined_pixels;

int rgb_backlight_save_state(int transition_ms);
int rgb_backlight_get_on_state(bool *state);

void rgb_backlight_tick(struct k_work *work);

// NOTE: initialize_blending_fns should be called before initialize_modes
void rgb_backlight_initialize_blending_fns();
void rgb_backlight_initialize_modes();
