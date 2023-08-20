#include "widgets/headers/layer_menu.h"
#include "widgets/headers/menu.h"
#include "widgets/headers/debug_output.h"
#include "status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct widget_layer_menu layer_menu_widget;
static struct widget_menu menu_widget;
static struct widget_debug_output debug_output_widget;
struct display_screens screens;

lv_obj_t *zmk_display_status_screen() {
	screens.main = lv_obj_create(NULL);
	screens.menu = lv_obj_create(NULL);
	lv_obj_set_scrollbar_mode(screens.main, LV_SCROLLBAR_MODE_ACTIVE);

	widget_menu_init(&menu_widget, screens.menu);
	lv_label_create(screens.main);

	// widget_debug_output_init(&debug_output_widget,main_screen);
	// lv_obj_t *debug_output_obj = widget_debug_output_obj(&debug_output_widget);
	// lv_obj_align(debug_output_obj, LV_ALIGN_TOP_RIGHT, 0, -10);

	return screens.main;
}
