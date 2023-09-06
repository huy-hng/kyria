#include "display/widgets/layer_menu.h"
#include "display/widgets/menu.h"
#include "display/widgets/debug_output.h"
#include "display/status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct display_screens screens;

lv_obj_t *zmk_display_status_screen() {
	screens.main = lv_obj_create(NULL);
	lv_obj_set_scrollbar_mode(screens.main, LV_SCROLLBAR_MODE_ACTIVE);

	static struct widget_debug_output debug_output_widget;
	lv_obj_t *debug_output_obj = widget_debug_output_init(&debug_output_widget, screens.main);
	// lv_obj_align(debug_output_obj, LV_ALIGN_TOP_LEFT, 0, -10);

	return screens.main;
}
