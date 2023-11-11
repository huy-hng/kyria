#include "display/widgets/peripheral_status.h"
#include "display/widgets/debug_output.h"
#include "display/status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

LV_IMG_DECLARE(kyria_logo);

static struct zmk_widget_peripheral_status peripheral_status_widget;
struct display_screens screens;

lv_obj_t *zmk_display_status_screen() {
	screens.main = lv_obj_create(NULL);
	screens.menu = lv_obj_create(NULL);
	screens.debug = lv_obj_create(NULL);

	lv_obj_t *kyria_logo_icon;
	kyria_logo_icon = lv_img_create(screens.main);
	lv_img_set_src(kyria_logo_icon, &kyria_logo);

	lv_img_set_size_mode(kyria_logo_icon, LV_IMG_SIZE_MODE_REAL);
	lv_img_set_antialias(kyria_logo_icon, true);
	lv_obj_align(kyria_logo_icon, LV_ALIGN_CENTER, 0, 0);
	// lv_img_set_zoom(kyria_logo_icon, 256);

	lv_obj_t *peripheral_obj = zmk_widget_peripheral_status_init(&peripheral_status_widget, screens.main);
	lv_obj_set_style_text_font(peripheral_obj, lv_theme_get_font_small(screens.main), LV_PART_MAIN);
	lv_obj_align(peripheral_obj, LV_ALIGN_TOP_RIGHT, 0, 0);

	// static struct widget_debug_output debug_output_widget;
	// lv_obj_t *debug_output_obj = widget_debug_output_init(&debug_output_widget, screens);
	// lv_obj_align(debug_output_obj, LV_ALIGN_TOP_LEFT, 0, 0);


#if IS_ENABLED(CONFIG_WIDGET_DEBUG_SCREEN)
	static struct widget_debug_output debug_output_widget;
	lv_obj_t *debug_output_obj = widget_debug_output_init(&debug_output_widget, screens.debug);
	lv_obj_align(debug_output_obj, LV_ALIGN_TOP_LEFT, 0, 0);
#endif

	return screens.main;
}
