#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <drivers/behavior.h>
#include <dt-bindings/zmk/rgb.h>

#include <zmk/keymap.h>
#include <zmk/behavior.h>
#include <zmk/ble.h>
// #include <zmk/rgb_underglow.h>
#include <zmk/event_manager.h>
#include <zmk/hid.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/split/bluetooth/central.h>

#include "rgb_underglow.h"

#define DEFAULT_BRIGHTNESS 30
#define DEFAULT_EFFECT 3
#define DEFAULT_COLOR CYAN

struct color {
    uint16_t h;
    uint8_t s;
    float b;
};

struct layer_color {
    char *label;
    struct color color;
    int effect;
};

static struct rgb_underglow_state base_state;
static int prev_layer_index;

// clang-format off
struct color desat  = {          .s = 60,.b = 0.8f};
struct color white  = {          .s = 1, .b = 0.6f};
struct color orange = {.h =   8,         .b = 0.8f};
struct color green  = {.h = 118,         .b = 0.8f};
struct color cyan   = {.h = 142,         .b = 0.8f};
struct color blue   = {.h = 192,         .b = 0.6f};
struct color indigo = {.h = 256,                  };
struct color pink   = {.h = 300,                  };
struct layer_color layers[20];
int layer_color_init() {

    layers[0] = (struct layer_color){.label = "Colemak", .effect = 3};
    layers[1] = (struct layer_color){.label = "Navipad",  indigo};
    layers[2] = (struct layer_color){.label = "Vim",      indigo};
    layers[3] = (struct layer_color){.label = "Symbols",  cyan};
    // layers[4] = (struct layer_color){.label = "Media FN", orange};
    layers[5] = (struct layer_color){.label = "Layers",   white};
    layers[6] = (struct layer_color){.label = "Media FN", green};
    layers[7] = (struct layer_color){.label = "OS",       blue};
    layers[8] = (struct layer_color){.label = "Enc LR",   orange};
    // clang-format on
    return 0;
}

struct layer_color *get_layer_color(const char *layer_label) {
    for (int i = 0; i < sizeof(layers) / sizeof(layers[0]); i++) {
        if (strcmp(layer_label, layers[i].label) == 0) {
            return &layers[i];
        }
    }
    return NULL;
}

void invoke_behavior(int param1, int param2) {
    struct zmk_behavior_binding binding = {
        .behavior_dev = "RGB_UG", .param1 = param1, .param2 = param2};

    struct zmk_behavior_binding_event event = {
        .layer = 0,
        .position = 0,
        .timestamp = 0,
    };

    for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
        zmk_split_bt_invoke_behavior(i, &binding, event, true);
    }
    behavior_keymap_binding_pressed(&binding, event);
}

void set_effect(int effect) { invoke_behavior(RGB_EFS_CMD, effect); }

void set_color(struct color color) {
    LOG_DBG(" ");
    LOG_DBG("hue %d", color.h);
    LOG_DBG("sat %d", color.s);
    LOG_DBG("bri %d", color.s);

    if (!color.h)
        color.h = base_state.color.h;
    if (!color.s)
        color.s = base_state.color.s;
    if (!color.b) {
        color.b = base_state.color.b;
    } else if (color.b > 0 && color.b < 1) {
        color.b = (uint8_t)base_state.color.b * color.b;
    }

    LOG_DBG("after hue %d", color.h);
    LOG_DBG("after sat %d", color.s);
    LOG_DBG("after bri %d", color.s);

    invoke_behavior(RGB_COLOR_HSB_CMD, RGB_COLOR_HSB_VAL(color.h, color.s, color.b));
}

void update_layer_color() {
    uint8_t index = zmk_keymap_highest_layer_active();
    const char *label = zmk_keymap_layer_label(index);
    const char *prev_layer_label = zmk_keymap_layer_label(prev_layer_index);

    if (prev_layer_index == 0 || strcmp(prev_layer_label, "Settings") == 0)
        base_state = zmk_rgb_underglow_return_state();

    prev_layer_index = index;

    struct layer_color *layer = get_layer_color(label);
    if (!layer)
        return;

    if (index == 0) {
        set_effect(base_state.current_effect);
        set_color((struct color){});
        return;
    }

    set_color(layer->color);
    set_effect(0);
}

int layer_color_event_listener(const zmk_event_t *eh) {
    // return 0;
    if (strcmp(eh->event->name, "zmk_layer_state_changed") == 0) {
        update_layer_color();
        return 0;
    }

    // NOTE: theres no distinction between modifier color change and layer color change
    // they will therefore mix and behave unexpectedly
    // also key release has to be handled more gracefully

    const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
    if (ev) {
        uint8_t mods = zmk_hid_get_explicit_mods();
        // struct rgb_underglow_state state = zmk_rgb_underglow_return_state();
        if (ev->state && mods > 0) {
            invoke_behavior(RGB_SET_SAT, 60);
            // invoke_behavior(RGB_SET_BRT, state.color.b * 0.8);
        } else if (!ev->state && mods == 0) {
            invoke_behavior(RGB_SET_SAT, 100);
            // invoke_behavior(RGB_SET_SAT, base_state.color.s);
            // invoke_behavior(RGB_SET_BRT, base_state.color.b);
        }
    }

    return 0;
}

ZMK_LISTENER(color, layer_color_event_listener);
ZMK_SUBSCRIPTION(color, zmk_layer_state_changed);
// ZMK_SUBSCRIPTION(color, zmk_keycode_state_changed);

SYS_INIT(layer_color_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
