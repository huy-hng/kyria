#include "core/lv_group.h"
#define DT_DRV_COMPAT zmk_behavior_display_navigation

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <lvgl.h>
#include <zmk/behavior.h>

#include <zmk/events/position_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int behavior_none_init(const struct device *dev) { return 0; };

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
									 struct zmk_behavior_binding_event event) {

	lv_group_t *group = lv_group_get_default();
	switch (binding->param1) {
	case UP: {
		lv_group_send_data(group, LV_KEY_UP);
		break;
	}
	case DOWN: {
		lv_group_send_data(group, LV_KEY_DOWN);
		break;
	}
	case LEFT: {
		lv_group_send_data(group, LV_KEY_LEFT);
		break;
	}
	case RIGHT: {
		lv_group_send_data(group, LV_KEY_RIGHT);
		break;
	}
	case ENTER: {
		lv_group_send_data(group, LV_KEY_ENTER);
		break;
	}
	case F1: {
		lv_obj_t *obj = lv_group_get_focused(group);
		lv_event_send(obj, LV_EVENT_CLICKED, NULL);
		break;
	}
	case F2: {
		bool is_editing = lv_group_get_editing(group);
		lv_group_set_editing(group, !is_editing);
		break;
	}
	}
	return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
									  struct zmk_behavior_binding_event event) {
	return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_none_driver_api = {
	.binding_pressed = on_keymap_binding_pressed,
	.binding_released = on_keymap_binding_released,
};

DEVICE_DT_INST_DEFINE(0, behavior_none_init, NULL, NULL, NULL, APPLICATION,
					  CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_none_driver_api);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
