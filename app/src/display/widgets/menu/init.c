#include <stdlib.h>

#include <zephyr/init.h>
#include <zmk/display.h>
#include <zmk/sensors.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/events/sensor_event.h>
#include <zmk/events/layer_state_changed.h>

#include "utils.h"
#include "display/status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include "display/widgets/menu.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static int last_layer_index = 0;

struct component_obj roller;
struct component_obj slider;
struct component_obj arc;
struct component_obj settings;

struct menu_state {
	int layer_index;
	const char *layer_label;
};

static struct menu_state menu_get_state(const zmk_event_t *eh) {
	uint8_t index = zmk_keymap_highest_layer_active();
	return (struct menu_state){
		.layer_index = index,
		.layer_label = zmk_keymap_layer_label(index),
	};
}

static void menu_update_cb(struct menu_state state) {
	if (state.layer_index == last_layer_index)
		return;

	if (state.layer_index == DISPLAY_MENU) {
		show_menu_main();
		show_screen(screens.menu);

	} else if (state.layer_index == ENC_MENU) {
		show_menu_encoder_modes(state.layer_index);
		show_screen(screens.menu);

	} else if (state.layer_index == LAYER_MENU) {
		show_menu_layers(state.layer_index);
		show_screen(screens.menu);

	} else if (state.layer_index == DEBUG_SCREEN) {
		show_menu_layers(state.layer_index);
		show_screen(screens.debug);
	} else {
		show_screen(screens.main);
	}
	last_layer_index = state.layer_index;
}

ZMK_DISPLAY_WIDGET_LISTENER(menu, struct menu_state, menu_update_cb, menu_get_state)
ZMK_SUBSCRIPTION(menu, zmk_layer_state_changed);

int widget_menu_init(struct widget_menu *widget, lv_obj_t *parent) {
	slider.container = create_container(parent);
	create_slider(&slider);
	hide_component(slider);

	arc.container = create_container(parent);
	create_arc(&arc);
	hide_component(arc);

	// settings.container = create_container(parent);
	// create_settings(&settings);
	// hide_component(settings);

	roller.container = create_container(parent);
	roller.last_index = 0;
	create_roller(&roller);
	show_component(roller);

	sys_slist_append(&widgets, &widget->node);
	// menu_init();

	return 0;
}
