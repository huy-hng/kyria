/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/events/keycode_state_changed.h>

#include <zmk/wpm.h>

#define CPS_UPDATE_INTERVAL_MS 500
#define CPS_RESET_INTERVAL_MS 1000

// static uint8_t cps_state = -1;
static uint8_t cps_state = -1;
static uint8_t last_cps_state;
static uint8_t cps_update_counter;
static uint8_t zero_counter;
static uint32_t key_presses_in_interval;
static uint32_t combined_key_presses;

int zmk_wpm_get_state() { return cps_state; }

int wpm_event_listener(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
    if (ev) {
        // count only key up events
        if (!ev->state) {
            key_presses_in_interval++;
            LOG_DBG("key_presses_in_interval %d keycode %d", key_presses_in_interval, ev->keycode);
        }
    }
    return 0;
}

void wpm_work_handler(struct k_work *work) {
    cps_update_counter++;
    // zero_counter++;
    combined_key_presses = combined_key_presses + key_presses_in_interval;

    float elapsed_time = cps_update_counter * (CPS_UPDATE_INTERVAL_MS / 1000.0);
    if (elapsed_time < 1) {
        elapsed_time = 1;
    }
    if (combined_key_presses == 0) {
        cps_update_counter = 1;
    }

    cps_state = combined_key_presses / elapsed_time;

    if (last_cps_state != cps_state) {
        LOG_DBG("Raised WPM state changed %d cps_update_counter %d", cps_state, cps_update_counter);

        ZMK_EVENT_RAISE(
            new_zmk_wpm_state_changed((struct zmk_wpm_state_changed){.state = cps_state}));

        last_cps_state = cps_state;
    }

    if (key_presses_in_interval == 0) {
        zero_counter++;
    } else {
        zero_counter = 0;
    }

    if (zero_counter * CPS_UPDATE_INTERVAL_MS >= CPS_RESET_INTERVAL_MS) {
        zero_counter = 0;
        cps_update_counter = 0;
        combined_key_presses = 0;
    }

    // if (cps_update_counter * CPS_UPDATE_INTERVAL_MS >= CPS_RESET_INTERVAL_MS) {
    //     zero_counter = 0;
    //     cps_update_counter = 1;
    //     combined_key_presses = key_presses_in_interval;
    // }

    key_presses_in_interval = 0;
}

K_WORK_DEFINE(wpm_work, wpm_work_handler);

void wpm_expiry_function() { k_work_submit(&wpm_work); }

K_TIMER_DEFINE(wpm_timer, wpm_expiry_function, NULL);

int wpm_init() {
    cps_state = 0;
    cps_update_counter = 0;
    k_timer_start(&wpm_timer, K_MSEC(CPS_UPDATE_INTERVAL_MS), K_MSEC(CPS_UPDATE_INTERVAL_MS));
    return 0;
}

ZMK_LISTENER(wpm, wpm_event_listener);
ZMK_SUBSCRIPTION(wpm, zmk_keycode_state_changed);

SYS_INIT(wpm_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
