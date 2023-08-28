#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>

#include <math.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>

#include <zephyr/drivers/led_strip.h>
#include <drivers/ext_power.h>

#include "rgb_backlight.h"

#include <zmk/activity.h>
#include <zmk/usb.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/workqueue.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if !DT_HAS_CHOSEN(zmk_underglow)
#error "A zmk,underglow chosen node must be declared"
#endif

BUILD_ASSERT(CONFIG_ZMK_RGB_UNDERGLOW_BRT_MIN <= CONFIG_ZMK_RGB_UNDERGLOW_BRT_MAX,
			 "ERROR: RGB underglow maximum brightness is less than minimum brightness");

const struct device *led_strip;
struct led_rgb pixels[STRIP_NUM_PIXELS];
struct rgb_backlight_state rgb_state;

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_EXT_POWER)
static const struct device *ext_power;
#endif

K_WORK_DEFINE(underglow_tick_work, rgb_backlight_tick);

static void rgb_backlight_tick_handler(struct k_timer *timer) {
	if (!rgb_state.on) {
		return;
	}

	k_work_submit_to_queue(zmk_workqueue_lowprio_work_q(), &underglow_tick_work);
}

K_TIMER_DEFINE(underglow_tick, rgb_backlight_tick_handler, NULL);

#if IS_ENABLED(CONFIG_SETTINGS)
static int rgb_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
	const char *next;
	int rc;

	if (settings_name_steq(name, "state", &next) && !next) {
		if (len != sizeof(rgb_state)) {
			return -EINVAL;
		}

		rc = read_cb(cb_arg, &rgb_state, sizeof(rgb_state));
		if (rc >= 0) {
			return 0;
		}

		return rc;
	}

	return -ENOENT;
}

struct settings_handler rgb_conf = {.name = "rgb/underglow", .h_set = rgb_settings_set};

static void rgb_backlight_save_state_work() {
	settings_save_one("rgb/underglow/state", &rgb_state, sizeof(rgb_state));
}

// TODO: make extern or export or whatever its called
static struct k_work_delayable underglow_save_work;
#endif

int rgb_backlight_save_state() {
#if IS_ENABLED(CONFIG_SETTINGS)
	int ret = k_work_reschedule(&underglow_save_work, K_MSEC(CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE));
	return MIN(ret, 0);
#else
	return 0;
#endif
}

int rgb_backlight_on() {
	if (!led_strip)
		return -ENODEV;

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_EXT_POWER)
	if (ext_power != NULL) {
		int rc = ext_power_enable(ext_power);
		if (rc != 0) {
			LOG_ERR("Unable to enable EXT_POWER: %d", rc);
		}
	}
#endif

	rgb_state.on = true;
	rgb_state.animation_step = 0;
	k_timer_start(&underglow_tick, K_NO_WAIT, K_MSEC(ANIMATION_REFRESH));

	return rgb_backlight_save_state();
}

static void rgb_backlight_off_handler(struct k_work *work) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		pixels[i] = (struct led_rgb){.r = 0, .g = 0, .b = 0};
	}

	led_strip_update_rgb(led_strip, pixels, STRIP_NUM_PIXELS);
}

K_WORK_DEFINE(underglow_off_work, rgb_backlight_off_handler);

int rgb_backlight_off() {
	if (!led_strip)
		return -ENODEV;

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_EXT_POWER)
	if (ext_power != NULL) {
		int rc = ext_power_disable(ext_power);
		if (rc != 0) {
			LOG_ERR("Unable to disable EXT_POWER: %d", rc);
		}
	}
#endif

	k_work_submit_to_queue(zmk_workqueue_lowprio_work_q(), &underglow_off_work);

	k_timer_stop(&underglow_tick);
	rgb_state.on = false;

	return rgb_backlight_save_state();
}

static int rgb_backlight_init(const struct device *_arg) {
	led_strip = DEVICE_DT_GET(STRIP_CHOSEN);

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_EXT_POWER)
	ext_power = device_get_binding("EXT_POWER");
	if (ext_power == NULL) {
		LOG_ERR("Unable to retrieve ext_power device: EXT_POWER");
	}
#endif

	rgb_state = (struct rgb_backlight_state){
		.color =
			{
				.h = CONFIG_ZMK_RGB_UNDERGLOW_HUE_START,
				.s = CONFIG_ZMK_RGB_UNDERGLOW_SAT_START,
				.b = CONFIG_ZMK_RGB_UNDERGLOW_BRT_START,
			},
		.animation_speed = CONFIG_ZMK_RGB_UNDERGLOW_SPD_START,
		.current_effect = CONFIG_ZMK_RGB_UNDERGLOW_EFF_START,
		.animation_step = 0,
		.on = IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_ON_START),
	};

#if IS_ENABLED(CONFIG_RGB_BACKLIGHT_LAYERS)
	layer_color_init();
#endif

#if IS_ENABLED(CONFIG_SETTINGS)
	settings_subsys_init();

	int err = settings_register(&rgb_conf);
	if (err) {
		LOG_ERR("Failed to register the ext_power settings handler (err %d)", err);
		return err;
	}

	k_work_init_delayable(&underglow_save_work, rgb_backlight_save_state_work);

	settings_load_subtree("rgb/underglow");
#endif

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB)
	rgb_state.on = zmk_usb_is_powered();
#endif

	if (rgb_state.on) {
		k_timer_start(&underglow_tick, K_NO_WAIT, K_MSEC(ANIMATION_REFRESH));
	}

	return 0;
}

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_IDLE) ||                                          \
	IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB)
static int rgb_backlight_auto_state(bool *prev_state, bool new_state) {
	if (rgb_state.on == new_state) {
		return 0;
	}
	if (new_state) {
		rgb_state.on = *prev_state;
		*prev_state = false;
		return rgb_backlight_on();
	} else {
		rgb_state.on = false;
		*prev_state = true;
		return rgb_backlight_off();
	}
}

static int rgb_backlight_event_listener(const zmk_event_t *eh) {

#if IS_ENABLED(CONFIG_RGB_BACKLIGHT_LAYERS)
	if (as_zmk_layer_state_changed(eh)) {
		return layer_color_event_listener(eh);
	}
#endif

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_IDLE)
	if (as_zmk_activity_state_changed(eh)) {
		static bool prev_state = false;
		return rgb_backlight_auto_state(&prev_state,
										zmk_activity_get_state() == ZMK_ACTIVITY_ACTIVE);
	}
#endif

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB)
	if (as_zmk_usb_conn_state_changed(eh)) {
		static bool prev_state = false;
		return rgb_backlight_auto_state(&prev_state, zmk_usb_is_powered());
	}
#endif

	return -ENOTSUP;
}

ZMK_LISTENER(rgb_backlight, rgb_backlight_event_listener);
#endif // IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_IDLE) ||
	   // IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB)

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_IDLE)
ZMK_SUBSCRIPTION(rgb_backlight, zmk_activity_state_changed);
#endif

#if IS_ENABLED(CONFIG_RGB_BACKLIGHT_LAYERS)
ZMK_SUBSCRIPTION(rgb_backlight, zmk_layer_state_changed);
#endif

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB)
ZMK_SUBSCRIPTION(rgb_backlight, zmk_usb_conn_state_changed);
#endif

SYS_INIT(rgb_backlight_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
