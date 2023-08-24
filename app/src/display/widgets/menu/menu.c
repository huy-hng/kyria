#include <zmk/keymap.h>
#include "../headers/menu.h"

#include "/home/huy/repositories/kyria/config/includes/generated/define_layers.dtsi"

void menu_testing() {
	zmk_keymap_layer_toggle(DISPLAY_MENU);
}

static void main_menu_event_handler(lv_event_t *e) {
	lv_obj_t *obj = lv_event_get_target(e);

	if (lv_event_get_key(e) == LV_KEY_ENTER) {
		int index = lv_roller_get_selected(obj);
		roller.last_index = index;

		switch (index) {
		case 0:
			return show_menu_rgb_brightness();
		case 1:
			return show_menu_rgb_hue();
		case 2:
			return show_menu_rgb_saturation();
		case 3:
			return show_menu_rgb_effects();
		case 4:
			return menu_testing();
		case 5: // exit menu
			zmk_keymap_layer_toggle(DISPLAY_MENU);
			return;
			// return show_menu_settings();
		};
	}
}

void show_menu_main() {
	char items[] = "Brt\n"
				   "Hue\n"
				   "Sat\n"
				   "Eff\n"
				   "Test\n"
				   "Exit"
		// "Set"
		;

	lv_label_set_text(roller.label, "Menu");
	set_new_event_cb(&roller, main_menu_event_handler, NULL);

	lv_roller_set_options(roller.obj, items, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_selected(roller.obj, roller.last_index, LV_ANIM_OFF);
}
