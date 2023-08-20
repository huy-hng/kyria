#include "widgets/headers/bongo_cat.h"
#include "widgets/headers/output_status.h"
#include "widgets/headers/peripheral_status.h"
#include "widgets/headers/layer_status.h"
#include "widgets/headers/battery_status.h"
#include "status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

lv_obj_t *zmk_display_status_screen() {
	lv_obj_t *screen;
	screen = lv_obj_create(NULL);
	// lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_ACTIVE);

#if IS_ENABLED(CONFIG_WIDGET_BONGO_CAT)
	static struct zmk_widget_bongo_cat bongo_cat_widget;
	lv_obj_t *bongo_cat = zmk_widget_bongo_cat_init(&bongo_cat_widget, screen);
	lv_obj_center(bongo_cat);
#endif

#if IS_ENABLED(CONFIG_WIDGET_BATTERY_STATUS)
	static struct zmk_widget_battery_status battery_status_widget;
	zmk_widget_battery_status_init(&battery_status_widget, screen);
	lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_WIDGET_OUTPUT_STATUS)
	static struct zmk_widget_output_status output_status_widget;
	zmk_widget_output_status_init(&output_status_widget, screen);
	lv_obj_set_style_text_font(zmk_widget_output_status_obj(&output_status_widget),
							   lv_theme_get_font_small(screen), LV_PART_MAIN);
	lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_WIDGET_LAYER_STATUS)
	// lv_obj_t *LayersHeading = lv_img_create(screen);
	// lv_obj_align(LayersHeading, LV_ALIGN_BOTTOM_MID, 0, -30);
	// lv_img_set_src(LayersHeading, &layers);

	LV_IMG_DECLARE(layers);
	static struct zmk_widget_layer_status layer_status_widget;
	zmk_widget_layer_status_init(&layer_status_widget, screen);
	lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_status_widget),
							   // lv_theme_get_font_small(screen),
							   &lv_font_montserrat_12, LV_PART_MAIN);
	lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_MID, 0, 0);
#endif

	return screen;
}
