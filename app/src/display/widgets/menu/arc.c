#include "../headers/menu.h"

#define STEP_SIZE 6

void arc_style(lv_style_t *style) {
	lv_style_init(style);
	lv_style_set_width(style, 64);
	lv_style_set_height(style, 64);
	lv_style_set_pad_all(style, 2);
	lv_style_set_border_width(style, 0);
}

void create_arc(lv_obj_t *parent) {
	arc.obj = lv_arc_create(parent);
	lv_arc_set_bg_angles(arc.obj, 0, 360);
	lv_arc_set_rotation(arc.obj, 270);

	static lv_style_t style;
	arc_style(&style);

	lv_obj_add_style(arc.obj, &style, LV_PART_MAIN);
	// lv_obj_remove_style(arc.obj, NULL, LV_PART_KNOB);
	lv_arc_set_range(arc.obj, 0, 360.0 / STEP_SIZE);

	// lv_obj_t *arc2 = lv_arc_create(parent);
	// lv_arc_set_bg_angles(arc2, 0, 180);
	// lv_arc_set_rotation(arc2, 180);
	// lv_obj_add_style(arc2, &style, LV_PART_MAIN);
	// lv_obj_remove_style(arc2, NULL, LV_PART_KNOB);
	// lv_obj_align(arc.obj, LV_ALIGN_LEFT_MID, 0, 0);

	arc.label = create_text(parent, "");
	arc.value_label = lv_label_create(parent);

	lv_obj_center(arc.obj);
	// lv_obj_align(arc.obj, LV_ALIGN_RIGHT_MID, 0, 0);
	// lv_obj_align(arc.obj, LV_ALIGN_CENTER, 0, 5);
	lv_obj_align(arc.label, LV_ALIGN_TOP_MID, 0, 4);
	lv_obj_align(arc.value_label, LV_ALIGN_CENTER, 0, 0);

	arc.group = lv_group_create();
	lv_group_add_obj(arc.group, arc.obj);
}

void set_arc_val(int value) {
	value /= STEP_SIZE;
	lv_arc_set_value(arc.obj, value);
	lv_label_set_text_fmt(arc.value_label, "%d°deg", value);
}

int get_arc_val() {
	int value = lv_arc_get_value(arc.obj);
	if (value == 360)
		value = 0;

	value *= STEP_SIZE;
	lv_label_set_text_fmt(arc.value_label, "%d°deg", value);
	return value;
}

void menu_arc_event_handler(lv_event_t *e) {
	void (*event_handler_cb)(int) = lv_event_get_user_data(e);
	// lv_label_set_text(arc.label, "o");
	if (should_exit_component(e, arc) || !has_value_updated(e))
		return;

	int value = get_arc_val();
	event_handler_cb(value);
}
