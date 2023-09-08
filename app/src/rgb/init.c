#include <zephyr/logging/log.h>
#include <zmk/usb.h>
#include <drivers/ext_power.h>

#include "rgb/rgb_backlight.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if !DT_HAS_CHOSEN(zmk_underglow)
#error "A zmk,underglow chosen node must be declared"
#endif

BUILD_ASSERT(CONFIG_ZMK_RGB_UNDERGLOW_BRT_MIN <= CONFIG_ZMK_RGB_UNDERGLOW_BRT_MAX,
			 "ERROR: RGB backlight maximum brightness is less than minimum brightness");

struct k_timer backlight_tick;
struct k_work backlight_tick_work;

const struct device *led_strip;
struct rgb_backlight_mode rgb_modes[rgb_mode_number];
uint8_t active_layer_index = 0;

struct rgb_backlight_pixel_range pixel_range = {
	.underglow = {.start = 0, .end = 6},
	.overglow = {.start = 6, .end = STRIP_NUM_PIXELS},
	.strip = {.start = 0, .end = STRIP_NUM_PIXELS},
};

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_EXT_POWER)
static const struct device *ext_power;
#endif

#if IS_ENABLED(CONFIG_SETTINGS)
static int rgb_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
	const char *next;
	int rc;

	if (settings_name_steq(name, "state", &next) && !next) {
		if (len != sizeof(rgb_modes[rgb_mode_base])) {
			return -EINVAL;
		}

		rc = read_cb(cb_arg, &rgb_modes[rgb_mode_base], sizeof(rgb_modes[rgb_mode_base]));
		if (rc >= 0) {
			return 0;
		}

		return rc;
	}

	return -ENOENT;
}

struct settings_handler rgb_conf = {.name = "rgb/backlight", .h_set = rgb_settings_set};

static void rgb_backlight_save_state_work() {
	settings_save_one("rgb/backlight/state", &rgb_modes[rgb_mode_base],
					  sizeof(rgb_modes[rgb_mode_base]));
}

static struct k_work_delayable backlight_save_work;
#endif

int rgb_backlight_save_state(int transition_ms) {

	// rgb_backlight_start_transition_effect(transition_ms);
#if IS_ENABLED(CONFIG_SETTINGS)
	int ret = k_work_reschedule(&backlight_save_work, K_MSEC(CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE));
	return MIN(ret, 0);
#else
	return 0;
#endif
}

int rgb_backlight_start() {
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

	rgb_modes[rgb_mode_base].on = true;
	rgb_modes[rgb_mode_base].animation_step = 0;
	k_timer_start(&backlight_tick, K_NO_WAIT, K_MSEC(CONFIG_RGB_REFRESH_MS));

	return rgb_backlight_save_state(500);
}

int rgb_backlight_stop() {
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

	k_timer_stop(&backlight_tick);
	rgb_modes[rgb_mode_base].on = false;

	return rgb_backlight_save_state(1000);
}

static int rgb_backlight_init(const struct device *_arg) {
	led_strip = DEVICE_DT_GET(STRIP_CHOSEN);

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_EXT_POWER)
	ext_power = device_get_binding("EXT_POWER");
	if (ext_power == NULL) {
		LOG_ERR("Unable to retrieve ext_power device: EXT_POWER");
	}
#endif
	rgb_backlight_initialize_modes();

	k_work_init(&backlight_tick_work, rgb_backlight_tick);
	k_timer_init(&backlight_tick, queue_lowprio_work, NULL);
	k_timer_user_data_set(&backlight_tick, &backlight_tick_work);

#if IS_ENABLED(CONFIG_SETTINGS)
	settings_subsys_init();

	int err = settings_register(&rgb_conf);
	if (err) {
		LOG_ERR("Failed to register the ext_power settings handler (err %d)", err);
		return err;
	}

	k_work_init_delayable(&backlight_save_work, rgb_backlight_save_state_work);

	settings_load_subtree("rgb/backlight");
#endif

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB)
	rgb_states[0].on = zmk_usb_is_powered();
#endif

	if (rgb_modes[rgb_mode_base].on)
		rgb_backlight_on();

	return 0;
}

SYS_INIT(rgb_backlight_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
