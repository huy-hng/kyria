#include "../headers/menu.h"

#define STEP_SIZE 2

void slider_style(lv_style_t *style) {
	lv_style_init(style);
	lv_style_init(style);
	lv_style_set_radius(style, 0);
	lv_style_set_pad_all(style, 2);
	lv_style_set_width(style, 80);
	lv_style_set_height(style, 10);
}


void create_slider(lv_obj_t *parent) {
	slider.obj = lv_slider_create(parent);

	lv_obj_remove_style(slider.obj, NULL, LV_PART_KNOB);

	static lv_style_t style;
	slider_style(&style);
	lv_obj_add_style(slider.obj, &style, LV_PART_MAIN);

	lv_slider_set_range(slider.obj, 0, 100.0 / STEP_SIZE);

	slider.label = create_text(parent, "");
	slider.value_label = lv_label_create(parent);

	lv_obj_center(slider.obj);
	lv_obj_align(slider.label, LV_ALIGN_TOP_MID, 0, 2);
	lv_obj_align(slider.value_label, LV_ALIGN_CENTER, 0, 15);

	slider.group = lv_group_create();
	lv_group_add_obj(slider.group, slider.obj);
}

int get_slider_val() {
	int value = lv_slider_get_value(slider.obj);
	value *= STEP_SIZE;
	lv_label_set_text_fmt(slider.value_label, "%d%%", value);
	return value;
}

void set_slider_val(int value, bool animate) {
	value /= STEP_SIZE;
	lv_slider_set_value(slider.obj, value, animate);
	lv_label_set_text_fmt(slider.value_label, "%d%%", value);
}

void menu_slider_event_handler(lv_event_t *e) {
	void (*event_handler_cb)(int) = lv_event_get_user_data(e);
	if (should_exit_component(e, slider) || !has_value_updated(e))
		return;

	int value = get_slider_val();
	// int value = get_arc_val();
	event_handler_cb(value);
}

