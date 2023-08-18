#include "widgets/headers/layer_menu.h"
#include "widgets/headers/debug_output.h"
#include "status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct widget_layer_menu layer_menu_widget;
static struct widget_debug_output debug_output_widget;

lv_obj_t *zmk_display_status_screen() {
	lv_obj_t *screen;
	screen = lv_obj_create(NULL);

	widget_debug_output_init(&debug_output_widget, screen);
	lv_obj_t *debug_output_obj = widget_debug_output_obj(&debug_output_widget);
	lv_obj_set_style_text_font(debug_output_obj, &lv_font_unscii_8, LV_PART_MAIN);
	lv_obj_align(debug_output_obj, LV_ALIGN_TOP_LEFT, 0, 0);

	// widget_layer_menu_init(&layer_menu_widget, screen);
	// lv_obj_t *layer_menu_obj = widget_layer_menu_obj(&layer_menu_widget);
	// lv_obj_set_style_text_font(layer_menu_obj, &lv_font_unscii_8, LV_PART_MAIN);
	// lv_obj_align(layer_menu_obj, LV_ALIGN_BOTTOM_MID, 0, 0);

	return screen;
}
