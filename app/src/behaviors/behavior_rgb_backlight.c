#define DT_DRV_COMPAT zmk_behavior_rgb_underglow

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/keymap.h>

#include <zmk/behavior.h>
#include <zmk/ble.h>
#include <zmk/split/bluetooth/central.h>

#include "../rgb/rgb_backlight.h"
#include "../utils.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);
#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int behavior_rgb_underglow_init(const struct device *dev) { return 0; }

static int
on_keymap_binding_convert_central_state_dependent_params(struct zmk_behavior_binding *binding,
														 struct zmk_behavior_binding_event event) {
	switch (binding->param1) {
	case RGB_TOG_CMD: {
		bool state;
		int err = rgb_backlight_get_on_state(&state);
		if (err) {
			LOG_ERR("Failed to get RGB underglow state (err %d)", err);
			return err;
		}

		binding->param1 = state ? RGB_OFF_CMD : RGB_ON_CMD;
		break;
	}
	case RGB_BRI_CMD: {
		struct zmk_led_hsb color = rgb_backlight_calc_brt(1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_BRD_CMD: {
		struct zmk_led_hsb color = rgb_backlight_calc_brt(-1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_HUI_CMD: {
		struct zmk_led_hsb color = rgb_backlight_calc_hue(1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_HUD_CMD: {
		struct zmk_led_hsb color = rgb_backlight_calc_hue(-1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_SAI_CMD: {
		struct zmk_led_hsb color = rgb_backlight_calc_sat(1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_SAD_CMD: {
		struct zmk_led_hsb color = rgb_backlight_calc_sat(-1);

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, color.s, color.b);
		break;
	}
	case RGB_EFR_CMD: {
		binding->param1 = RGB_EFS_CMD;
		binding->param2 = rgb_backlight_calc_effect(-1);
		break;
	}
	case RGB_EFF_CMD: {
		binding->param1 = RGB_EFS_CMD;
		binding->param2 = rgb_backlight_calc_effect(1);
		break;
	}
	case RGB_SET_HUE: {
		struct rgb_backlight_state state = *rgb_backlight_get_state();
		struct zmk_led_hsb color = state.color;

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(binding->param2, color.s, color.b);
		break;
	}
	case RGB_SET_SAT: {
		struct rgb_backlight_state state = *rgb_backlight_get_state();
		struct zmk_led_hsb color = state.color;

		binding->param1 = RGB_COLOR_HSB_CMD;
		binding->param2 = RGB_COLOR_HSB_VAL(color.h, binding->param2, color.b);
		break;
	}
	case RGB_SET_BRT: {
		struct rgb_backlight_state state = *rgb_backlight_get_state();
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

	// _send_to_peripheral(binding, event);
	switch (binding->param1) {
	case RGB_TOG_CMD:
		rgb_backlight_toggle();
		break;
	case RGB_ON_CMD:
		rgb_backlight_on();
		break;
	case RGB_OFF_CMD:
		rgb_backlight_off();
		break;
	case RGB_HUI_CMD:
		rgb_backlight_change_hue(1);
		break;
	case RGB_HUD_CMD:
		rgb_backlight_change_hue(-1);
		break;
	case RGB_SAI_CMD:
		rgb_backlight_change_sat(1);
		break;
	case RGB_SAD_CMD:
		rgb_backlight_change_sat(-1);
		break;
	case RGB_BRI_CMD:
		rgb_backlight_change_brt(1);
		break;
	case RGB_BRD_CMD:
		rgb_backlight_change_brt(-1);
		break;
	case RGB_SPI_CMD:
		rgb_backlight_change_spd(1);
		break;
	case RGB_SPD_CMD:
		rgb_backlight_change_spd(-1);
		break;
	case RGB_EFS_CMD:
		rgb_backlight_select_effect(binding->param2, &rgb_state);
		break;
	case RGB_EFF_CMD:
		rgb_backlight_cycle_effect(1);
		break;
	case RGB_EFR_CMD:
		rgb_backlight_cycle_effect(-1);
		break;
	case RGB_COLOR_HSB_CMD:
		rgb_backlight_set_hsb((struct zmk_led_hsb){.h = (binding->param2 >> 16) & 0xFFFF,
												   .s = (binding->param2 >> 8) & 0xFF,
												   .b = binding->param2 & 0xFF},
							  &rgb_state);
		break;
	case RGB_COLOR_HSB_LAYER_CMD:
		rgb_backlight_set_hsb((struct zmk_led_hsb){.h = (binding->param2 >> 16) & 0xFFFF,
												   .s = (binding->param2 >> 8) & 0xFF,
												   .b = binding->param2 & 0xFF},
							  &layer_color_state);
		break;
	case RGB_SET_HUE:
		rgb_backlight_set_hue(binding->param2);
		break;
	case RGB_SET_SAT:
		rgb_backlight_set_sat(binding->param2);
		break;
	case RGB_SET_BRT:
		rgb_backlight_set_brt(binding->param2);
		break;
	case RGB_EFS_UDG:
		rgb_underglow_select_effect(binding->param2);
		break;
	}

	rgb_backlight_start_transition_effect();
	return 0; // HACK: remove this hack

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
