#include "widgets/headers/peripheral_status.h"

#include "status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

LV_IMG_DECLARE(kyria_logo);

static struct zmk_widget_peripheral_status peripheral_status_widget;

lv_obj_t *zmk_display_status_screen() {
	lv_obj_t *screen;
	screen = lv_obj_create(NULL);

	lv_obj_t *kyria_logo_icon;
	kyria_logo_icon = lv_img_create(screen);
	lv_img_set_src(kyria_logo_icon, &kyria_logo);

	lv_img_set_size_mode(kyria_logo_icon, LV_IMG_SIZE_MODE_REAL);
	lv_img_set_antialias(kyria_logo_icon, true);
	lv_obj_align(kyria_logo_icon, LV_ALIGN_CENTER, 0, 0);
	// lv_img_set_zoom(kyria_logo_icon, 256);

	zmk_widget_peripheral_status_init(&peripheral_status_widget, screen);
	lv_obj_set_style_text_font(zmk_widget_peripheral_status_obj(&peripheral_status_widget),
							   lv_theme_get_font_small(screen), LV_PART_MAIN);
	lv_obj_align(zmk_widget_peripheral_status_obj(&peripheral_status_widget), LV_ALIGN_TOP_RIGHT, 0,
				 0);

	return screen;
}
