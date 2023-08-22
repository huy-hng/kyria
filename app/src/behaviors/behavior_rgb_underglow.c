/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_rgb_underglow

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <dt-bindings/zmk/rgb.h>
#include <zmk/keymap.h>

#include <zmk/behavior.h>
#include <zmk/ble.h>
#include <zmk/split/bluetooth/central.h>

#include "../rgb/rgb_extra.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

// send_to_peripheral(RGB_COLOR_HSB_CMD, RGB_COLOR_HSB_VAL(color.h, color.s, color.b));

static int behavior_rgb_underglow_init(const struct device *dev) { return 0; }

static int
on_keymap_binding_convert_central_state_dependent_params(struct zmk_behavior_binding *binding,
														 struct zmk_behavior_binding_event event) {
	switch (binding->param1) {
	case RGB_TOG_CMD: {
		bool state;
		int err = zmk_rgb_underglow_get_state(&state);
		if (err) {
			LOG_ERR("Failed to get RGB underglow state (err %d)", err);
			return err;
		}

		binding->param1 = state ? RGB_OFF_CMD : RGB_ON_CMD;
		break;
	}
	case RGB_BRI_CMD: {
		struct zmk_led_hsb color = zmk_rgb_underglow_calc_brt(1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_BRD_CMD: {
		struct zmk_led_hsb color = zmk_rgb_underglow_calc_brt(-1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_HUI_CMD: {
		struct zmk_led_hsb color = zmk_rgb_underglow_calc_hue(1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_HUD_CMD: {
		struct zmk_led_hsb color = zmk_rgb_underglow_calc_hue(-1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_SAI_CMD: {
		struct zmk_led_hsb color = zmk_rgb_underglow_calc_sat(1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_SAD_CMD: {
		struct zmk_led_hsb color = zmk_rgb_underglow_calc_sat(-1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_EFR_CMD: {
		binding->param1 = RGB_EFS_CMD;
		binding->param2 = zmk_rgb_underglow_calc_effect(-1);
		break;
	}
	case RGB_EFF_CMD: {
		binding->param1 = RGB_EFS_CMD;
		binding->param2 = zmk_rgb_underglow_calc_effect(1);
		break;
	}
	case RGB_SET_HUE: {
		struct rgb_underglow_state_extra state = *zmk_rgb_underglow_return_state();
		struct zmk_led_hsb color = state.color;

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(binding->param2, color.s, color.b);
		break;
	}
	case RGB_SET_SAT: {
		struct rgb_underglow_state_extra state = *zmk_rgb_underglow_return_state();
		struct zmk_led_hsb color = state.color;

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, binding->param2, color.b);
		break;
	}
	case RGB_SET_BRT: {
		struct rgb_underglow_state_extra state = *zmk_rgb_underglow_return_state();
		struct zmk_led_hsb color = state.color;

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, binding->param2);
		break;
	}
	default:
		return 0;
	}

	LOG_DBG("RGB relative convert to absolute (%d/%d)", binding->param1, binding->param2);

	return 0;
};

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
									 struct zmk_behavior_binding_event event) {
	switch (binding->param1) {
	case RGB_TOG_CMD:
		return zmk_rgb_underglow_toggle();
	case RGB_ON_CMD:
		return zmk_rgb_underglow_on();
	case RGB_OFF_CMD:
		return zmk_rgb_underglow_off();
	case RGB_HUI_CMD:
		return zmk_rgb_underglow_change_hue(1);
	case RGB_HUD_CMD:
		return zmk_rgb_underglow_change_hue(-1);
	case RGB_SAI_CMD:
		return zmk_rgb_underglow_change_sat(1);
	case RGB_SAD_CMD:
		return zmk_rgb_underglow_change_sat(-1);
	case RGB_BRI_CMD:
		return zmk_rgb_underglow_change_brt(1);
	case RGB_BRD_CMD:
		return zmk_rgb_underglow_change_brt(-1);
	case RGB_SPI_CMD:
		return zmk_rgb_underglow_change_spd(1);
	case RGB_SPD_CMD:
		return zmk_rgb_underglow_change_spd(-1);
	case RGB_EFS_CMD:
		zmk_rgb_underglow_select_effect(binding->param2);
		rgb_extra_start_transition_animation();
		return 0;
	case RGB_EFF_CMD:
		return zmk_rgb_underglow_cycle_effect(1);
	case RGB_EFR_CMD:
		return zmk_rgb_underglow_cycle_effect(-1);
	case RGB_COLOR_HSB_CMD:
		return zmk_rgb_underglow_set_hsb((struct zmk_led_hsb){.h = (binding->param2 >> 16) & 0xFFFF,
															  .s = (binding->param2 >> 8) & 0xFF,
															  .b = binding->param2 & 0xFF});
	case RGB_SET_HUE:
		return zmk_rgb_underglow_set_hue(binding->param2);
	case RGB_SET_SAT:
		return zmk_rgb_underglow_set_sat(binding->param2);
	case RGB_SET_BRT:
		return zmk_rgb_underglow_set_brt(binding->param2);
	}

	return -ENOTSUP;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
									  struct zmk_behavior_binding_event event) {
	return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_rgb_underglow_driver_api = {
	.binding_convert_central_state_dependent_params =
		on_keymap_binding_convert_central_state_dependent_params,
	.binding_pressed = on_keymap_binding_pressed,
	.binding_released = on_keymap_binding_released,
	.locality = BEHAVIOR_LOCALITY_GLOBAL,
};

DEVICE_DT_INST_DEFINE(0, behavior_rgb_underglow_init, NULL, NULL, NULL, APPLICATION,
					  CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_rgb_underglow_driver_api);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
