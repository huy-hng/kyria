#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_bongo_cat {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_anim_t anim;
    lv_obj_t *tapping;
};
void start_idle_animation();
void change_state_paws_up();
void lift_paws();
void hit_table();

lv_obj_t * zmk_widget_bongo_cat_init(struct zmk_widget_bongo_cat *widget, lv_obj_t *parent);
