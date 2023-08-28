#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct widget_debug_output {
	sys_snode_t node;
	lv_obj_t *obj;
};

void debug_set_text(char *text);
void debug_set_text_fmt(char *fmt, ...);

void debug_append_text(char *text);
void debug_append_text_fmt(char *fmt, ...);

void debug_newline_text(char *text);
void debug_newline_text_fmt(char *fmt, ...);

lv_obj_t *widget_debug_output_init(struct widget_debug_output *widget, lv_obj_t *parent);
