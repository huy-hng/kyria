#include "../headers/menu.h"

void show_component(struct component_obj obj) {
	lv_group_set_default(obj.group);
	lv_obj_clear_flag(obj.container, LV_OBJ_FLAG_HIDDEN);
}

void hide_component(struct component_obj obj) {
	lv_obj_add_flag(obj.container, LV_OBJ_FLAG_HIDDEN);
}

void set_new_event_cb(struct component_obj *obj, void (*event_handler_cb)(), void *user_data) {
	lv_obj_remove_event_dsc(obj->obj, obj->last_event_cb);
	obj->last_event_cb = lv_obj_add_event_cb(obj->obj, event_handler_cb, LV_EVENT_ALL, user_data);
}

void initialize_group(struct component_obj *component, bool set_default) {
	component->group = lv_group_create();
	lv_group_add_obj(component->group, component->obj);
	if (set_default)
		lv_group_set_default(component->group);
}

bool has_value_updated(lv_event_t *e) { return lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED; }

bool enter_pressed(lv_event_t *e) {
	if (lv_event_get_key(e) == LV_KEY_ENTER)
		return true;
	return false;
}

bool any_key_pressed(lv_event_t *e) {
	if (lv_event_get_code(e) == LV_EVENT_KEY)
		return true;
	return false;
}

bool should_exit_component(lv_event_t *e, struct component_obj obj) {
	if (enter_pressed(e)) {
		hide_component(obj);
		show_component(roller);
		return true;
	}
	return false;
}

lv_obj_t *create_container(lv_obj_t *parent) {
	lv_obj_t *container = lv_obj_create(parent);
	lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_ACTIVE);
	lv_obj_set_style_border_width(container, 0, 0);
	lv_obj_set_size(container, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
	lv_obj_center(container);
	return container;
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
