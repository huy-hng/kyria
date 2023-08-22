#include "widgets/headers/bongo_cat.h"
#include "widgets/headers/output_status.h"
#include "widgets/headers/peripheral_status.h"
#include "widgets/headers/layer_status.h"
#include "widgets/headers/battery_status.h"
#include "widgets/headers/menu.h"

#include "widgets/headers/debug_output.h"
#include "status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct display_screens screens;

lv_obj_t *zmk_display_status_screen() {
	screens.main = lv_obj_create(NULL);
	screens.menu = lv_obj_create(NULL);
	// lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_ACTIVE);

#if IS_ENABLED(CONFIG_WIDGET_BONGO_CAT)
	static struct zmk_widget_bongo_cat bongo_cat_widget;
	lv_obj_t *bongo_cat = zmk_widget_bongo_cat_init(&bongo_cat_widget, screens.main);
	lv_obj_align(bongo_cat, LV_ALIGN_CENTER, 0, -5);
#endif

#if IS_ENABLED(CONFIG_WIDGET_BATTERY_STATUS)
	static struct zmk_widget_battery_status battery_status_widget;
	lv_obj_t *battery_status = zmk_widget_battery_status_init(&battery_status_widget, screens.main);
	lv_obj_align(battery_status, LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_WIDGET_OUTPUT_STATUS)
	static struct zmk_widget_output_status output_status_widget;
	lv_obj_t *output_status = zmk_widget_output_status_init(&output_status_widget, screens.main);
	lv_obj_set_style_text_font(output_status, lv_theme_get_font_small(screens.main), LV_PART_MAIN);
	lv_obj_align(output_status, LV_ALIGN_TOP_LEFT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_WIDGET_LAYER_STATUS)
	static struct zmk_widget_layer_status layer_status_widget;
	lv_obj_t *layer_status = zmk_widget_layer_status_init(&layer_status_widget, screens.main);
	lv_obj_align(layer_status, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_style_text_font(layer_status, &lv_font_montserrat_12, LV_PART_MAIN);
	// lv_theme_get_font_small(parent),
#endif

#if IS_ENABLED(CONFIG_WIDGET_MENU)
	static struct widget_menu menu_widget;
	widget_menu_init(&menu_widget, screens.menu);
#endif

	// static struct widget_debug_output debug_output_widget;
	// lv_obj_t *debug_output_obj = widget_debug_output_init(&debug_output_widget, screens.menu);
	// lv_obj_align(debug_output_obj, LV_ALIGN_TOP_LEFT, 0, 0);

	return screens.main;
}
