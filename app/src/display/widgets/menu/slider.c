#include "display/widgets/menu.h"

#define STEP_SIZE 2

void slider_style(lv_style_t *style) {
	lv_style_init(style);
	lv_style_init(style);
	lv_style_set_radius(style, 0);
	lv_style_set_pad_all(style, 2);
	lv_style_set_width(style, 80);
	lv_style_set_height(style, 10);
}

void create_slider(struct component_obj *slider) {
	slider->obj = lv_slider_create(slider->container);

	lv_obj_remove_style(slider->obj, NULL, LV_PART_KNOB);

	static lv_style_t style;
	slider_style(&style);
	lv_obj_add_style(slider->obj, &style, LV_PART_MAIN);

	lv_slider_set_range(slider->obj, 0, 100.0 / STEP_SIZE);

	slider->label = create_text(slider->container, "");
	slider->value_label = lv_label_create(slider->container);

	lv_obj_center(slider->obj);
	lv_obj_align(slider->label, LV_ALIGN_TOP_MID, 0, 2);
	lv_obj_align(slider->value_label, LV_ALIGN_CENTER, 0, 15);

	initialize_group(slider, false);
}

int get_slider_val() {
	int value = lv_slider_get_value(slider.obj);
	value *= STEP_SIZE;
	lv_label_set_text_fmt(slider.value_label, "%d%%", value);
	return value;
}

void set_slider_val(int value, bool animate) {
	lv_label_set_text_fmt(slider.value_label, "%d%%", value);
	lv_slider_set_value(slider.obj, value / STEP_SIZE, animate);
}

void menu_slider_event_handler(lv_event_t *e) {
	// struct menu_work_container *menu_work = (struct menu_work_container *)lv_event_get_user_data(e);
	// void (*event_handler_cb)(int) = lv_event_get_user_data(e);
	if (should_exit_component(e, slider) || !has_value_updated(e))
		return;

	menu_rgb_work.value = get_slider_val();
	k_work_reschedule(&menu_rgb_work.delayable_work, K_MSEC(MENU_SLIDER_DEBOUNCE));
}
