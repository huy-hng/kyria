#include "display/widgets/menu.h"
#include "display/status_screen.h"

void menu_testing() { zmk_keymap_layer_toggle(DISPLAY_MENU); }

typedef struct {
	char *name;
	void (*(fn))();
} menu_item;

menu_item items[4];

static void main_menu_event_handler(lv_event_t *e) {
	lv_obj_t *obj = lv_event_get_target(e);

	if (lv_event_get_key(e) == LV_KEY_ENTER) {
		int index = lv_roller_get_selected(obj);
		roller.last_index = index;

		items[index].fn();
	}
}
char *get_menu_item_names(char *text) {
	for (int i = 0; i < 4; i++) {
		if (i > 0)
			strcat(text, "\n");
		sprintf(text, "%s%s", text, items[i].name);
	}
	return text;
}

void exit_display_menu() { //
	zmk_keymap_layer_deactivate(DISPLAY_MENU);
}

void show_debug_screen() {
	show_screen(screens.debug);
	exit_display_menu();
}

void show_menu_main() {
	items[0] = (menu_item){.name = "Brt", .fn = show_menu_rgb_brightness};
	items[1] = (menu_item){.name = "Hue", .fn = show_menu_rgb_hue};
	items[2] = (menu_item){.name = "Sat", .fn = show_menu_rgb_saturation};
	items[3] = (menu_item){.name = "Eff", .fn = show_menu_rgb_effects};
	// items[4] = (menu_item){.name = "Exit", .fn = exit_display_menu};
	// items[5] = (menu_item){.name = "Bug", .fn = show_debug_screen};

	lv_label_set_text(roller.label, "Menu");
	set_new_event_cb(&roller, main_menu_event_handler, NULL);
	char text[20] = "";
	get_menu_item_names(text);

	lv_roller_set_options(roller.obj, text, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_selected(roller.obj, roller.last_index, LV_ANIM_OFF);
}
