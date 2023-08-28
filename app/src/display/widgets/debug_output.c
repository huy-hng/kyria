#include "widgets/lv_label.h"
#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "headers/debug_output.h"
#include <zmk/event_manager.h>

static struct widget_debug_output *DEBUG_WIDGET;
static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
bool initialized = false;

static char *debug_text;

void debug_set_text(char *text) { //
	if (!initialized)
		return;
	lv_label_set_text(DEBUG_WIDGET->obj, text);
}

void debug_set_text_fmt(char *fmt, ...) {
	char new_text[500];

	va_list args;
	va_start(args, fmt);
	vsprintf(new_text, fmt, args);

	debug_set_text(new_text);
	va_end(args);
}

void debug_append_text(char *text) {
	if (!initialized)
		return;

	char *current_text = lv_label_get_text(DEBUG_WIDGET->obj);
	char new_text[500];

	sprintf(new_text, "%s%s", current_text, text);
	debug_set_text(new_text);
}

void debug_append_text_fmt(char *fmt, ...) {
	char new_text[500];

	va_list args;
	va_start(args, fmt);

	vsprintf(new_text, fmt, args);

	// char *current_text = lv_label_get_text(DEBUG_WIDGET->obj);
	// sprintf(new_text, "%s%s", current_text, new_text);
	// lv_label_set_text(DEBUG_WIDGET->obj, new_text);

	debug_append_text(new_text);
	va_end(args);
}

void debug_newline_text(char *text) {
	if (!initialized)
		return;

	char *current_text = lv_label_get_text(DEBUG_WIDGET->obj);
	char new_text[500];

	sprintf(new_text, "%s\n%s", current_text, text);
	debug_set_text(new_text);
}

void debug_newline_text_fmt(char *fmt, ...) {
	char new_text[500];

	va_list args;
	va_start(args, fmt);

	vsprintf(new_text, fmt, args);

	debug_newline_text(new_text);
	va_end(args);
}

lv_obj_t *widget_debug_output_init(struct widget_debug_output *widget, lv_obj_t *parent) {
	widget->obj = lv_label_create(parent);
	DEBUG_WIDGET = widget;
	initialized = true;

	debug_set_text("");

	sys_slist_append(&widgets, &widget->node);

	return widget->obj;
}
