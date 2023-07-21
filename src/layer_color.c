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
#include <zmk/rgb_underglow.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/split/bluetooth/central.h>

#define ORANGE 8
#define GREEN 118
#define CYAN 128
#define BLUE 192
#define INDIGO 256
#define PINK 300

struct layer_color {
    char *label;
    int effect;
    struct zmk_led_hsb color;
    // int hue;
    // int sat;
    // int bri;
};

struct layer_color base = {.label = "BASE"};
struct layer_color navipad = {.label = "NAVIPAD", .color = {INDIGO}};
struct layer_color symbols = {.label = "SYMBOLS", .color = {GREEN}};

uint8_t prev_effect;

struct rgb_underglow_state {
    struct zmk_led_hsb color;
    uint8_t animation_speed;
    uint8_t current_effect;
    uint16_t animation_step;
    bool on;
};

struct zmk_behavior_binding_event event = {
    .layer = 0,
    .position = 0,
    .timestamp = 0,
};

struct zmk_behavior_binding create_binding(int param1, int param2) {
    return (struct zmk_behavior_binding){
        .behavior_dev = "RGB_UG", .param1 = param1, .param2 = param2};
}

void invoke_behavior(int param1, int param2) {
    struct zmk_behavior_binding binding = {
        .behavior_dev = "RGB_UG", .param1 = param1, .param2 = param2};

    for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
        zmk_split_bt_invoke_behavior(i, &binding, event, true);
    }
    behavior_keymap_binding_pressed(&binding, event);
}

void set_effect(int effect) {
    invoke_behavior(RGB_EFS_CMD, effect);
}

void set_color(struct zmk_led_hsb color) {
    struct rgb_underglow_state *state = zmk_rgb_underglow_return_state();

    if (!color.h)
        color.h = state->color.h;
    if (!color.s)
        color.s = state->color.s;
    if (!color.b)
        color.b = state->color.b;

    int bitshifted = ((color.h) << 16) + ((color.s) << 8) + (color.b);
    invoke_behavior(RGB_COLOR_HSB_CMD, bitshifted);
}

int layer_color_event_listener(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    const char *label = zmk_keymap_layer_label(index);

    struct zmk_behavior_binding binding = {
        .behavior_dev = "RGB_UG", .param1 = RGB_EFS_CMD, .param2 = 0};

    struct zmk_behavior_binding binding2 = {
        .behavior_dev = "RGB_UG", .param1 = RGB_EFS_CMD, .param2 = 3};

    struct zmk_behavior_binding_event event = {
        .layer = 0,
        .position = 0,
        .timestamp = 0,
    };
    if (index > 1) {
        // behavior_keymap_binding_pressed(&binding, event);
        zmk_rgb_underglow_select_effect(0);
        for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
            zmk_split_bt_invoke_behavior(i, &binding, event, true);
        }
        // zmk_rgb_underglow_set_hue(INDIGO);
    } else {

        zmk_rgb_underglow_select_effect(3);
        // behavior_keymap_binding_pressed(&binding2, event);
        for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
            zmk_split_bt_invoke_behavior(i, &binding2, event, true);
        }
    }

    return 0;
}

int layer_color_init() { return 0; }

ZMK_LISTENER(layer_color, layer_color_event_listener);
ZMK_SUBSCRIPTION(layer_color, zmk_layer_state_changed);

SYS_INIT(layer_color_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
