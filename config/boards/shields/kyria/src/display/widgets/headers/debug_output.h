#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct widget_debug_output {
	sys_snode_t node;
	lv_obj_t *obj;
	lv_obj_t *text;
};

void debug_set_text(char *text);
void debug_set_text_fmt(char *fmt, ...);

void debug_add_text(char *text);
void debug_add_text_fmt(char *fmt, ...);

int widget_debug_output_init(struct widget_debug_output *widget, lv_obj_t *parent);
lv_obj_t *widget_debug_output_obj(struct widget_debug_output *widget);
