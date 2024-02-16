#include "display/widgets/menu.h"

int layer_items[] = {
	BASE,
	GAMING,
	FIDGET,
	SUDOKU,
	QWERTY,
	BASE_CAPS,
	NO_HOLD,
	TESTING,
	DEBUG_SCREEN,
};

static void menu_layers_event_handler(lv_event_t *e) {
	int len =  sizeof(layer_items) / sizeof(layer_items[0]); 

	if (any_key_pressed(e)) {
		for (int i = 1; i < len; i++)
			zmk_keymap_layer_deactivate(layer_items[i]);

		int index = lv_roller_get_selected(roller.obj);
		zmk_keymap_layer_activate(layer_items[index]);

		// lv_label_set_text_fmt(roller.label, "%d", index);
	}
}

void show_menu_layers(int layer_index) {
	int len =  sizeof(layer_items) / sizeof(layer_items[0]); 
	char items[100] = "Base";

	int active_index = 0;
	for (int i = 1; i < len; i++) {
		if (zmk_keymap_layer_active(layer_items[i]))
			active_index = i;

		const char *label = zmk_keymap_layer_label(layer_items[i]);

		sprintf(items, "%s\n%s", items, label);
		// sprintf(items, i == 0 ? "%s%s" : "%s\n%s", items, label);
	}

	lv_label_set_text(roller.label, "Layers");
	set_new_event_cb(&roller, menu_layers_event_handler, NULL);

	lv_roller_set_options(roller.obj, items, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_selected(roller.obj, active_index, LV_ANIM_OFF);

	hide_component(slider);
	hide_component(arc);
	show_component(roller);
}
