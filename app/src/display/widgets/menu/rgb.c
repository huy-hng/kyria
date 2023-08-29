#include "../../../rgb/rgb_backlight.h"
#include "../headers/menu.h"

int rgb_backlight_select_effect(int);

static void menu_rgb_effects_event_handler(lv_event_t *e) {
	if (lv_event_get_key(e) == LV_KEY_ENTER)
		return show_menu_main();

	if (lv_event_get_code(e) == LV_EVENT_KEY) {
		lv_obj_t *obj = lv_event_get_target(e);

		if (!rgb_state.on)
			rgb_backlight_on();

		int index = lv_roller_get_selected(obj);

		send_to_peripheral(RGB_UG, RGB_EFS_CMD, index);
		rgb_backlight_select_effect(index);
	}
}

void show_menu_rgb_effects() {
	char items[] = "Solid\n"
				   "Breath\n"
				   "Spectr\n"
				   "Swirl\n"
				   "Sparkl";

	lv_label_set_text(roller.label, "RGB Effects");
	lv_roller_set_options(roller.obj, items, LV_ROLLER_MODE_INFINITE);

	struct rgb_backlight_state rgb_state = *rgb_backlight_get_state();
	lv_roller_set_selected(roller.obj, rgb_state.current_effect, LV_ANIM_OFF);

	set_new_event_cb(&roller, menu_rgb_effects_event_handler, NULL);
}

void show_menu_rgb_brightness() {
	struct rgb_backlight_state rgb_state = *rgb_backlight_get_state();
	set_slider_val(rgb_state.color.b, 0);

	lv_label_set_text(slider.label, "Brightness");
	hide_component(roller);
	show_component(slider);

	set_new_event_cb(&slider, menu_slider_event_handler, rgb_backlight_set_brt);
}
void show_menu_rgb_saturation() {
	struct rgb_backlight_state rgb_state = *rgb_backlight_get_state();
	set_slider_val(rgb_state.color.s, 0);

	lv_label_set_text(slider.label, "Saturation");
	hide_component(roller);
	show_component(slider);

	set_new_event_cb(&slider, menu_slider_event_handler, rgb_backlight_set_sat);
}

void show_menu_rgb_hue() {
	struct rgb_backlight_state rgb_state = *rgb_backlight_get_state();
	set_arc_val(rgb_state.color.h);

	lv_label_set_text(arc.label, "Hue");
	hide_component(roller);
	show_component(arc);

	set_new_event_cb(&arc, menu_arc_event_handler, rgb_backlight_set_hue);
}
