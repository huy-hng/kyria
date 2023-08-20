#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/sensors.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/events/sensor_event.h>
#include <zmk/events/layer_state_changed.h>

#include "headers/menu.h"
#include "headers/debug_output.h"
#include "../status_screen.h"
#include "../../rgb/rgb_extra.h"
#include <zmk/rgb_underglow.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static int last_timestamp;
static int rotation_angle;

struct menu_state {
	const struct zmk_position_state_changed *position;
	const struct zmk_sensor_event *sensor;
};

static void event_handler(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);

	if (code == LV_EVENT_VALUE_CHANGED) {
		char buf[32];
		lv_roller_get_selected_str(obj, buf, sizeof(buf));
		int i = lv_roller_get_selected(obj);

		zmk_rgb_underglow_select_effect(i);
		rgb_extra_start_transition_animation();

		debug_set_text_fmt("%s %d", buf, i);
	}
}

lv_obj_t *create_menu(lv_obj_t *roller, const char *menu_items) {
	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_anim_time(&style, 300);
	lv_style_set_border_width(&style, 0);
	lv_style_set_pad_hor(&style, 2);
	// lv_style_set_border_side(&style, LV_BORDER_SIDE_TOP);
	lv_style_set_text_letter_space(&style, -1);

	// lv_style_set_outline_width(&style, 2);
	// lv_style_set_outline_pad(&style, 5);
	lv_obj_add_style(roller, &style, 0);

	lv_roller_set_options(roller, menu_items, LV_ROLLER_MODE_INFINITE);

	// lv_obj_set_size(roller1, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
	// lv_obj_set_height(roller1, lv_disp_get_ver_res(NULL));
	lv_roller_set_visible_row_count(roller, 3);
	lv_obj_add_event_cb(roller, event_handler, LV_EVENT_ALL, NULL);

	lv_group_t *group = lv_group_create();
	lv_group_set_default(group);
	lv_group_add_obj(lv_group_get_default(), roller);
	return roller;
}

int parse_rotation(const struct zmk_sensor_event *sensor) {
	const struct zmk_sensor_config *config = zmk_sensors_get_config_at_index(sensor->sensor_index);
	uint16_t triggers_per_rotation = config->triggers_per_rotation;
	double angle_per_trigger = 360.0 / triggers_per_rotation;

	int timestamp = sensor->timestamp;
	int diff = timestamp - last_timestamp;

	bool valid_rotation = diff < 500; // TODO: put this somewhere else
	int rotation_val = sensor->channel_data->value.val1;

	rotation_angle = valid_rotation ? rotation_angle + rotation_val : 0;
	// debug_set_text_fmt("%d %d %d", diff, rotation_angle, rotation_val);

	int action = 0;
	if (abs(rotation_angle) >= angle_per_trigger) {
		action = rotation_angle > 0 ? LV_KEY_DOWN : LV_KEY_UP;
		rotation_angle = 0;
	}

	last_timestamp = timestamp;
	return action;
}

static void set_menu(lv_obj_t *obj, struct menu_state state) {
	if (state.sensor->channel_data->channel == SENSOR_CHAN_ROTATION) {
		int action = parse_rotation(state.sensor);
		if (action != 0) {
			lv_group_send_data(lv_group_get_default(), action);
		}
	} else if (state.position->position == ENCODER_POSITION) {
		lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
	}
}

static struct menu_state menu_get_state(const zmk_event_t *eh) {
	return (struct menu_state){
		.position = as_zmk_position_state_changed(eh),
		.sensor = as_zmk_sensor_event(eh),
	};
}

int menu_update_cb(const zmk_event_t *eh) {
	if (lv_scr_act() != screens.menu)
		return 0;

	struct menu_state state = menu_get_state(eh);

	struct widget_menu *widget;
	SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
		// lv_async_call(my_screen_clean_up, obj);
		set_menu(widget->obj, state);
	}
	return 0;
}

static void menu_update_cb_display(struct menu_state state) {
	struct widget_menu *widget;
	SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_menu(widget->obj, state); }
}

int show_menu_cb(const zmk_event_t *eh) {
	uint8_t index = zmk_keymap_highest_layer_active();
	const char *label = zmk_keymap_layer_label(index);

	// TODO: put this somewhere else
	if (strcmp(label, "Settings Menu") == 0) {
		show_screen(screens.menu);
	} else {
		show_screen(screens.main);
	}
	return 0;
}

lv_obj_t *create_line(lv_obj_t *parent) {
	lv_obj_t *line = lv_line_create(parent);

	static lv_point_t line_points[] = {{0, 0}, {75, 0}};
	lv_line_set_points(line, line_points, 2);

	lv_obj_set_style_line_width(line, 2, 0);
	return line;
}

lv_obj_t *create_text(lv_obj_t *parent, char *text) {
	lv_obj_t *label = lv_label_create(parent);
	lv_label_set_text(label, text);
	// lv_obj_set_size(label, 120, 10);
	// lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_style_text_letter_space(label, -1, 0);

	return label;
}

int widget_menu_init(struct widget_menu *widget, lv_obj_t *parent) {
	char items[] = "Solid\n"
				   "Breath\n"
				   "Spectr\n"
				   "Swirl\n"
				   "Sparkl";

	widget->obj = create_menu(lv_roller_create(parent), items);

	lv_obj_align(widget->obj, LV_ALIGN_BOTTOM_MID, 0, 0);
	// lv_obj_set_style_text_font(widget->obj, &lv_font_unscii_8, LV_PART_MAIN);
	// lv_coord_t height = lv_obj_get_height(widget->obj);

	lv_obj_t *label = create_text(parent, "RGB Effect");
	lv_obj_align_to(label, widget->obj, LV_ALIGN_OUT_TOP_MID, 0, -3);

	lv_obj_t *line = create_line(parent);
	lv_obj_align_to(line, widget->obj, LV_ALIGN_OUT_TOP_MID, 2, 2);

	sys_slist_append(&widgets, &widget->node);

	// menu_init();
	return 0;
}

// ZMK_DISPLAY_WIDGET_LISTENER(menu, struct menu_state, menu_update_cb, menu_get_state)
ZMK_LISTENER(menu, menu_update_cb)

ZMK_SUBSCRIPTION(menu, zmk_sensor_event);
ZMK_SUBSCRIPTION(menu, zmk_position_state_changed);

ZMK_LISTENER(show_menu, show_menu_cb)
ZMK_SUBSCRIPTION(show_menu, zmk_layer_state_changed);
