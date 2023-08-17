#include "widgets/headers/peripheral_status.h"

#include "status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

LV_IMG_DECLARE(zenlogo);

static struct zmk_widget_peripheral_status peripheral_status_widget;

lv_obj_t *zmk_display_status_screen() {
	lv_obj_t *screen;
	screen = lv_obj_create(NULL);

	zmk_widget_peripheral_status_init(&peripheral_status_widget, screen);
	lv_obj_set_style_text_font(zmk_widget_peripheral_status_obj(&peripheral_status_widget),
							   lv_theme_get_font_small(screen), LV_PART_MAIN);
	lv_obj_align(zmk_widget_peripheral_status_obj(&peripheral_status_widget), LV_ALIGN_TOP_LEFT, 0,
				 0);

	// lv_obj_t *zenlogo_icon;
	// zenlogo_icon = lv_img_create(screen);
	// lv_img_set_src(zenlogo_icon, &zenlogo);
	// lv_obj_align(zenlogo_icon, LV_ALIGN_CENTER, 0, 0);
	return screen;
}
