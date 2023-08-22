#include "../headers/menu.h"

// TODO: maybe use meter components?
// colorwheel also makes sense and potentially allows to wrap around 360

#define STEP_SIZE 6
#define ARC_WIDTH 5
#define ARC_BORDER_WIDTH 1

void arc_style_bg(lv_style_t *style) {
	lv_style_init(style);
	lv_style_set_width(style, 72);
	lv_style_set_height(style, 72);
	
	lv_style_set_arc_width(style, ARC_WIDTH);
	// lv_style_set_pad_all(style, 10);
	lv_style_set_border_width(style, 0);
}

void arc_style_fg(lv_style_t *style) {
	lv_style_init(style);
	// lv_style_set_width(style, 55);
	// lv_style_set_height(style, 55);

	lv_style_set_arc_width(style, ARC_WIDTH - ARC_BORDER_WIDTH * 2);
	// lv_style_set_arc_color(style, lv_color_white());
	lv_style_set_arc_color_filtered(style, lv_color_white());
	lv_style_set_pad_all(style, ARC_BORDER_WIDTH);
	lv_style_set_border_width(style, 4);
}

void create_arc(lv_obj_t *parent) {
	arc.obj = lv_arc_create(parent);
	lv_arc_set_mode(arc.obj, LV_ARC_MODE_REVERSE);

	lv_arc_set_bg_angles(arc.obj, 0, 360);
	lv_arc_set_angles(arc.obj, 0, 360);
	lv_arc_set_rotation(arc.obj, 270);

	static lv_style_t style_bg;
	arc_style_bg(&style_bg);

	static lv_style_t style_fg;
	arc_style_fg(&style_fg);

	lv_obj_add_style(arc.obj, &style_bg, LV_PART_MAIN);
	lv_obj_add_style(arc.obj, &style_fg, LV_PART_INDICATOR);

	lv_obj_remove_style(arc.obj, NULL, LV_PART_KNOB);
	lv_arc_set_range(arc.obj, 0, 360.0 / STEP_SIZE);

	arc.label = create_text(parent, "");
	arc.value_label = lv_label_create(parent);
	lv_obj_set_style_text_font(arc.value_label, &lv_font_montserrat_16, LV_PART_MAIN);
	lv_obj_set_style_text_letter_space(arc.value_label, 1, 0);

	lv_obj_center(arc.obj);
	lv_obj_align(arc.label, LV_ALIGN_TOP_MID, 0, 10);
	lv_obj_align(arc.value_label, LV_ALIGN_CENTER, 3, 3);

	arc.group = lv_group_create();
	lv_group_add_obj(arc.group, arc.obj);
}

void set_arc_val(int value) {
	lv_arc_set_value(arc.obj, value / STEP_SIZE);
	lv_label_set_text_fmt(arc.value_label, "%d°", value);
}

int get_arc_val() {
	int value = lv_arc_get_value(arc.obj);
	if (value == 360)
		value = 0;

	value *= STEP_SIZE;
	lv_label_set_text_fmt(arc.value_label, "%d°", value);
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
