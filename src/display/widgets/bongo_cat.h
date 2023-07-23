#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zmk/display/widgets/wpm_status.h>

struct zmk_widget_bongo_cat {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_obj_t *debug;
    lv_anim_t anim;
    // lv_obj_t *paws_up;
    lv_obj_t *tapping;
};

int zmk_widget_bongo_cat_init(struct zmk_widget_bongo_cat *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_bongo_cat_obj(struct zmk_widget_bongo_cat *widget);
