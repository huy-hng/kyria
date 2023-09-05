#pragma once

#include <lvgl.h>
lv_obj_t *zmk_display_status_screen();

struct display_screens {
	lv_obj_t *main;
	lv_obj_t *menu;
	lv_obj_t *debug;
} extern screens;

static void show_screen(lv_obj_t *screen) {
	if (lv_scr_act() != screen)
		lv_scr_load(screen);
}
