#include "widgets/lv_label.h"
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "display/widgets/debug_output.h"
#include <zmk/event_manager.h>

static struct widget_debug_output *DEBUG_WIDGET;
static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static char debug_text[150];
bool initialized = false;

// display can display 7 lines and 17 columns (when letter space is -1)
// TODO: maybe use span instead?

static void wrap_text(int line_len) {
	// int curr_line_len = 0;
	// for (int i = 0; strlen(debug_text); i++) {
	// 	if (debug_text[i] == '\n')
	// 		curr_line_len = 0;
	// 	curr_line_len++;
	// }

	char *d;
	char *dest = d;
	char *s = debug_text;

	while (*s != '\0') {
		*d = *s;
		d++;
		s++;
	}

	*d = '\0';
	strcpy(debug_text, dest);
}

void debug_update_text() {
	if (!initialized)
		return;
	// wrap_text(17);
	lv_label_set_text_static(DEBUG_WIDGET->obj, NULL);
}

void debug_set_text(char *text) {
	strcpy(debug_text, text);
	debug_update_text();
}

void debug_append_text(char *text) { strcat(debug_text, text); }

void debug_newline_text(char *text) {
	strcat(debug_text, "\n");
	strcat(debug_text, text);
}

void debug_set_text_fmt(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	vsnprintf(debug_text, sizeof(debug_text), fmt, args);
	debug_update_text();

	va_end(args);
}

void debug_append_text_fmt(char *fmt, ...) {
	char new_text[112];

	va_list args;
	va_start(args, fmt);
	char *format = "%s";

	vsnprintf(new_text, sizeof(new_text), fmt, args);
	strcat(debug_text, new_text);

	va_end(args);
}

void debug_newline_text_fmt(char *fmt, ...) {
	char new_text[112];

	va_list args;
	va_start(args, fmt);

	vsnprintf(new_text, sizeof(new_text), fmt, args);

	strcat(debug_text, "\n");
	strcat(debug_text, new_text);

	va_end(args);
}

lv_obj_t *widget_debug_output_init(struct widget_debug_output *widget, lv_obj_t *parent) {
	widget->obj = lv_label_create(parent);
	DEBUG_WIDGET = widget;
	initialized = true;

	lv_label_set_text_static(DEBUG_WIDGET->obj, debug_text);
	lv_obj_set_style_text_letter_space(widget->obj, -1, 0);

	debug_set_text("  Debug Screen");

	sys_slist_append(&widgets, &widget->node);

	return widget->obj;
}
