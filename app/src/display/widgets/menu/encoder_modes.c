#include "display/widgets/menu.h"

int enc_layers[] = {BASE, ENC_LR, ENC_TAB};

static void menu_encoder_modes_event_handler(lv_event_t *e) {
	int len = sizeof(enc_layers) / sizeof(enc_layers[0]); 

	if (lv_event_get_code(e) == LV_EVENT_KEY) {
		for (int i = 1; i < len; i++)
			zmk_keymap_layer_deactivate(enc_layers[i]);

		int index = lv_roller_get_selected(roller.obj);
		zmk_keymap_layer_activate(enc_layers[index]);

		// lv_label_set_text_fmt(roller.label, "%d", index);
	}
}

void show_menu_encoder_modes(int layer_index) {
	int len = sizeof(enc_layers) / sizeof(enc_layers[0]); 
	char items[100] = "Base";

	int active_index = 0;
	for (int i = 1; i < len; i++) {
		if (zmk_keymap_layer_active(enc_layers[i]))
			active_index = i;

		const char *label = zmk_keymap_layer_label(enc_layers[i]);

		sprintf(items, "%s\n%s", items, label);
		// sprintf(items, i == 0 ? "%s%s" : "%s\n%s", items, label);
	}

	lv_label_set_text(roller.label, "Encoder Mode");
	set_new_event_cb(&roller, menu_encoder_modes_event_handler, NULL);

	lv_roller_set_options(roller.obj, items, LV_ROLLER_MODE_INFINITE);

	lv_roller_set_selected(roller.obj, active_index, LV_ANIM_OFF);

	hide_component(slider);
	hide_component(arc);
	show_component(roller);
}
