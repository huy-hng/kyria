#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zmk/keymap.h>
#include "imports.h"

typedef struct _lv_event_dsc_t event_cb;

struct widget_menu {
	sys_snode_t node;
	lv_obj_t *obj;
	lv_obj_t *menu;
	lv_obj_t *slider;
};

struct component_obj {
	lv_obj_t *container;
	lv_obj_t *obj;
	lv_obj_t *label;
	lv_group_t *group;
	lv_obj_t *value_label;
	int last_index;
	event_cb *last_event_cb;
};

// globals (seems super hacky, is this how to c? :o)
extern struct component_obj roller;
extern struct component_obj slider;
extern struct component_obj arc;
extern struct component_obj settings;

int widget_menu_init(struct widget_menu *widget, lv_obj_t *parent);

// components
lv_obj_t *create_container(lv_obj_t *parent);
lv_obj_t *create_line(lv_obj_t *parent);
lv_obj_t *create_text(lv_obj_t *parent, char *text);
void initialize_group(struct component_obj *component, bool set_default);

void show_component(struct component_obj obj);
void hide_component(struct component_obj obj);
void set_new_event_cb(struct component_obj *obj, void (*event_handler_cb)(), void *user_data);
bool enter_pressed(lv_event_t *e);
bool any_key_pressed(lv_event_t *e);
bool has_value_updated(lv_event_t *e);
bool should_exit_component(lv_event_t *e, struct component_obj obj);

// roller
void create_roller(struct component_obj *roller);

// slider
void create_slider(struct component_obj *slider);
int get_slider_val();
void set_slider_val(int value, bool animate);
void menu_slider_event_handler(lv_event_t *e);

// arc
void create_arc(struct component_obj *arc);
int get_arc_val();
void set_arc_val(int value);
void menu_arc_event_handler(lv_event_t *e);

void show_menu_main();
void show_menu_rgb_effects();
void show_menu_rgb_brightness();
void show_menu_rgb_saturation();
void show_menu_rgb_hue();
void show_menu_layers(int layer_index);
void show_menu_settings();
void show_menu_encoder_modes(int layer_index);

// settings
void create_settings(struct component_obj *settings);

