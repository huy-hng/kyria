#include "rgb/rgb_backlight.h"
#include "display/widgets/menu.h"


static void menu_rgb_effects_event_handler(lv_event_t *e) {
	if (lv_event_get_key(e) == LV_KEY_ENTER)
		return show_menu_main();

	if (lv_event_get_code(e) == LV_EVENT_KEY) {
		lv_obj_t *obj = lv_event_get_target(e);

		if (!rgb_modes[rgb_mode_base].on)
			rgb_backlight_on();

		int index = lv_roller_get_selected(obj);

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
		send_to_peripheral(RGB_UG, RGB_EFS_CMD, index);
#endif
		rgb_backlight_select_effect(index, &rgb_modes[rgb_mode_base]);
	}
}

void show_menu_rgb_effects() {
	char items[] = "Off\n"
				   "Solid\n"
				   "Breath\n"
				   "Spectr\n"
				   "Swirl\n"
				   "Sparkl";

	lv_label_set_text(roller.label, "RGB Effects");
	lv_roller_set_options(roller.obj, items, LV_ROLLER_MODE_INFINITE);

	lv_roller_set_selected(roller.obj, rgb_modes[rgb_mode_base].active_animation, LV_ANIM_OFF);

	set_new_event_cb(&roller, menu_rgb_effects_event_handler, NULL);
}

void show_menu_rgb_brightness() {
	set_slider_val(rgb_modes[rgb_mode_base].color.b, false);

	lv_label_set_text(slider.label, "Brightness");
	hide_component(roller);
	show_component(slider);

	menu_rgb_work.function = rgb_backlight_set_brt;
	set_new_event_cb(&slider, menu_slider_event_handler, NULL);
}

void show_menu_rgb_saturation() {
	set_slider_val(rgb_modes[rgb_mode_base].color.s, false);

	lv_label_set_text(slider.label, "Saturation");
	hide_component(roller);
	show_component(slider);

	menu_rgb_work.function = rgb_backlight_set_sat;
	set_new_event_cb(&slider, menu_slider_event_handler, NULL);
}

void show_menu_rgb_hue() {
	set_arc_val(rgb_modes[rgb_mode_base].color.h);

	lv_label_set_text(arc.label, "Hue");
	hide_component(roller);
	show_component(arc);

	menu_rgb_work.function = rgb_backlight_set_hue;
	set_new_event_cb(&arc, menu_arc_event_handler, NULL);
}
