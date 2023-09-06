#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "display/widgets/peripheral_status.h"
#include <zmk/event_manager.h>
#include <zmk/split/bluetooth/peripheral.h>
#include <zmk/events/split_peripheral_status_changed.h>

LV_IMG_DECLARE(bluetooth_connected_right);
LV_IMG_DECLARE(bluetooth_disconnected_right);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct peripheral_status_state {
	bool connected;
};

static struct peripheral_status_state get_state(const zmk_event_t *_eh) {
	return (struct peripheral_status_state){.connected = zmk_split_bt_peripheral_is_connected()};
}

static void set_status_symbol(lv_obj_t *obj, struct peripheral_status_state state) {
	LOG_DBG("connected? %s", state.connected ? "true" : "false");

	// (LV_SYMBOL_WIFI " " LV_SYMBOL_OK) : (LV_SYMBOL_WIFI " " LV_SYMBOL_CLOSE);
	const char *text = state.connected ? LV_SYMBOL_WIFI : " " LV_SYMBOL_CLOSE;
	lv_label_set_text(obj, text);

	// lv_img_set_src(obj, state.connected ? &bluetooth_connected_right :
	// &bluetooth_disconnected_right);
}

static void output_status_update_cb(struct peripheral_status_state state) {
	struct zmk_widget_peripheral_status *widget;
	SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_status_symbol(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_status, struct peripheral_status_state,
							output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_peripheral_status, zmk_split_peripheral_status_changed);

lv_obj_t *zmk_widget_peripheral_status_init(struct zmk_widget_peripheral_status *widget,
											lv_obj_t *parent) {
	// widget->obj = lv_img_create(parent);
	widget->obj = lv_label_create(parent);

	sys_slist_append(&widgets, &widget->node);

	widget_peripheral_status_init();
	return widget->obj;
}
