#include "widgets/headers/layer_menu.h"
#include "status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct widget_layer_menu layer_menu_widget;

lv_obj_t *zmk_display_status_screen() {
	lv_obj_t *screen;
	screen = lv_obj_create(NULL);

	widget_layer_menu_init(&layer_menu_widget, screen);
	lv_obj_set_style_text_font(widget_layer_menu_obj(&layer_menu_widget), &lv_font_unscii_8,
							   LV_PART_MAIN);
	lv_obj_align(widget_layer_menu_obj(&layer_menu_widget), LV_ALIGN_BOTTOM_MID, 0, 0);

	return screen;
}
