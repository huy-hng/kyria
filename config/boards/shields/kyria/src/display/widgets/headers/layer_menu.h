#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct widget_layer_menu {
    sys_snode_t node;
    lv_obj_t *obj;
	lv_obj_t *text;
};

int widget_layer_menu_init(struct widget_layer_menu *widget, lv_obj_t *parent);
lv_obj_t *widget_layer_menu_obj(struct widget_layer_menu *widget);
