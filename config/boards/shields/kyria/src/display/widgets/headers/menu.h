#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct widget_menu {
    sys_snode_t node;
    lv_obj_t *obj;
	lv_obj_t *text;
};

static int ENCODER_POSITION = 32;

int widget_menu_init(struct widget_menu *widget, lv_obj_t *parent);
lv_obj_t *widget_menu_obj(struct widget_menu *widget);
