#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "headers/layer_menu.h"
#include <zmk/sensors.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>

#include <zmk/events/sensor_event.h>

int ENCODER_POSITION = 32;

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct widget_layer_menu *WIDGET;

struct layer_menu_state {
	uint8_t index;
	int64_t timestamp;
	int action;
	struct zmk_sensor_channel_data data;
};

int timestamp;
static lv_obj_t *list;

void set_text(char *txt) { lv_label_set_text(WIDGET->text, txt); }

static void event_handler(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);
	

	char text[20] = {};
	// sprintf(text, "%i %i", code, timestamp);
	if (code < 18) {
		// sprintf(text, "%s", "arst");
		sprintf(text, "%s %i %s", lv_list_get_btn_text(list, obj), code, (char*)e->user_data);
		// sprintf(text, "%s %i", lv_list_get_btn_text(list, obj), code);
		set_text(text);
	}
}

void add_button(lv_obj_t *list, char *icon, char *text) {
	lv_obj_t *list_btn;
	list_btn = lv_list_add_btn(list, icon, text);
	lv_group_add_obj(lv_group_get_default(), list_btn);
	lv_obj_add_event_cb(list_btn, event_handler, LV_EVENT_ALL, text);
}

lv_obj_t *create_list(lv_obj_t *parent) {
	list = lv_list_create(parent);
	lv_obj_set_size(list, 128, 64);
	lv_obj_set_style_pad_row(list, 5, 0);
	// lv_obj_set_style_pad_ver(list, 0, 0);
	// lv_obj_add_event_cb(list, event_handler, LV_EVENT_ALL, "list");

	lv_group_t *group = lv_group_create();
	lv_group_set_default(group);
	lv_group_add_obj(lv_group_get_default(), list);

	add_button(list, "", "Base");
	add_button(list, "", "Left/Right");
	add_button(list, "", "Tab");
	add_button(list, "", "Edit");
	add_button(list, "", "Save");
	add_button(list, "", "Notify");
	add_button(list, "", "Battery");
	return list;
}

static void set_layer_menu(lv_obj_t *label, struct layer_menu_state state) {
	lv_group_send_data(lv_group_get_default(), state.action);
}

static void layer_menu_update_cb(struct layer_menu_state state) {

	// TODO:
	// uint16_t triggers_per_rotation =
	// 	zmk_sensors_get_config_at_index(state.index)->triggers_per_rotation;

	struct widget_layer_menu *widget;
	SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
		// lv_async_call(my_screen_clean_up, lv_scr_act());
		set_layer_menu(widget->obj, state);
	}
}

static struct layer_menu_state layer_menu_get_state(const zmk_event_t *eh) {
	const struct zmk_sensor_event *sensor = as_zmk_sensor_event(eh);
	const struct zmk_position_state_changed *position = as_zmk_position_state_changed(eh);

	struct zmk_sensor_channel_data data = *sensor->channel_data;
	int action;

	char text[30];

	char *side = position->source ? "Right" : " Left";
	char *pressed = position->state ? "Down" : "UP  ";
	if (data.channel == SENSOR_CHAN_ROTATION) {
		if (data.value.val1 > 0) {
			// set_text("down");
			action = LV_KEY_DOWN;
		} else {
			// set_text("up");
			action = LV_KEY_UP;
		}
	} else if (position->position == ENCODER_POSITION && position->state) {
		// set_text("enter");
		action = LV_KEY_ENTER;
	}

	// sprintf(text, "%s %i %s", side, position->position, pressed);
	// sprintf(text, "%i %i %i", data.value.val1, data.channel, position->source);
	// set_text(text);

	timestamp = sensor->timestamp;
	return (struct layer_menu_state){
		.index = sensor->sensor_index,
		.timestamp = sensor->timestamp,
		.action = action,
		.data = *sensor->channel_data,
	};
}

ZMK_DISPLAY_WIDGET_LISTENER(layer_menu, struct layer_menu_state, layer_menu_update_cb,
							layer_menu_get_state)

ZMK_SUBSCRIPTION(layer_menu, zmk_sensor_event);
ZMK_SUBSCRIPTION(layer_menu, zmk_position_state_changed);

int widget_layer_menu_init(struct widget_layer_menu *widget, lv_obj_t *parent) {
	widget->obj = create_list(parent);
	widget->text = lv_label_create(parent);

	lv_obj_align(widget->text, LV_ALIGN_TOP_RIGHT, -10, 10);
	WIDGET = widget;

	sys_slist_append(&widgets, &widget->node);

	// layer_menu_init();
	return 0;
}

lv_obj_t *widget_layer_menu_obj(struct widget_layer_menu *widget) { return widget->obj; }
