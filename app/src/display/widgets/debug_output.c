#include "widgets/lv_label.h"
#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "headers/debug_output.h"
#include <zmk/event_manager.h>

static struct widget_debug_output *WIDGET;
static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
bool initialized = false;

static char *debug_text;

void debug_set_text(char *text) { //

	if (!initialized)
		return;
	char new_text[100];
	sprintf(new_text, "%s\n", text);

	lv_label_set_text(WIDGET->obj, new_text);
}

void debug_set_text_fmt(char *fmt, ...) {
	char new_text[100];

	va_list args;
	va_start(args, fmt);
	vsprintf(new_text, fmt, args);

	debug_set_text(new_text);
	va_end(args);
}

void debug_add_text(char *text) {
	if (!initialized)
		return;
	char *current_text = lv_label_get_text(WIDGET->obj);
	char new_text[100];

	sprintf(new_text, "%s%s", current_text, text);
	debug_set_text(new_text);
}

void debug_add_text_fmt(char *fmt, ...) {
	char new_text[100];

	va_list args;
	va_start(args, fmt);
	vsprintf(new_text, fmt, args);

	debug_add_text(new_text);
	va_end(args);
}

int widget_debug_output_init(struct widget_debug_output *widget, lv_obj_t *parent) {
	widget->obj = lv_label_create(parent);
	WIDGET = widget;
	initialized = true;

	debug_set_text("debug me");

	// char text[100] = "";
	// lv_label_set_text(WIDGET->obj, text);
	// debug_set_text("debug me");
	// debug_new_fmt_line("%s %d", "fmt", 987);
	// debug_new_fmt_line("%s %d", "fmt", 9);
	// debug_new_line("qwfp");

	sys_slist_append(&widgets, &widget->node);
	return 0;
}

lv_obj_t *widget_debug_output_obj(struct widget_debug_output *widget) { return widget->obj; }
