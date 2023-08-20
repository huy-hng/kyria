#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_layer_status {
    sys_snode_t node;
    lv_obj_t *obj;
};

lv_obj_t * zmk_widget_layer_status_init(struct zmk_widget_layer_status *widget, lv_obj_t *parent);
