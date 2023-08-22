#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/sensors.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/events/sensor_event.h>
#include <zmk/events/layer_state_changed.h>

// #include "headers/debug_output.h"
#include "../headers/menu.h"
#include "../../status_screen.h"
#include "../../../rgb/rgb_extra.h"
#include "../../../utils.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct component_obj roller;
struct component_obj slider;
struct component_obj arc;
struct component_obj settings;

struct menu_state {
	int layer_index;
	const char *layer_label;
};

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
		};
	}
}

void set_menu_main() {
	char items[] = "Brt\n"
				   "Hue\n"
				   "Sat\n"
				   "Eff";

	lv_label_set_text(roller.label, "Menu");
	set_new_event_cb(&roller, main_menu_event_handler, NULL);

	lv_roller_set_options(roller.obj, items, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_selected(roller.obj, roller.last_index, LV_ANIM_OFF);
}

int widget_menu_init(struct widget_menu *widget, lv_obj_t *parent) {
	slider.container = create_container(parent);
	create_slider(slider.container);
	hide_component(slider);

	arc.container = create_container(parent);
	arc.last_index = 0;
	create_arc(arc.container);
	hide_component(arc);

	roller.container = create_container(parent);
	roller.last_index = 0;
	create_roller(roller.container);
	show_component(roller);
	set_menu_main();

	sys_slist_append(&widgets, &widget->node);

	return 0;
}

static struct menu_state menu_get_state(const zmk_event_t *eh) {
	uint8_t index = zmk_keymap_highest_layer_active();
	return (struct menu_state){
		.layer_index = index,
		.layer_label = zmk_keymap_layer_label(index),
	};
}

static void menu_update_cb(struct menu_state state) {
	if (same_str(state.layer_label, "Display Menu")) {
		show_screen(screens.menu);
	} else {
		show_screen(screens.main);
	}
}

ZMK_DISPLAY_WIDGET_LISTENER(menu, struct menu_state, menu_update_cb, menu_get_state)
ZMK_SUBSCRIPTION(menu, zmk_layer_state_changed);
