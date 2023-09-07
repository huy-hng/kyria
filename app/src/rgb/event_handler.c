#include <zmk/keymap.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/position_state_changed.h>

#include "rgb/rgb_backlight.h"

// clang-format off
#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_IDLE) || IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB)
// clang-format on
static int rgb_backlight_auto_state(bool *prev_state, bool new_state) {
	if (rgb_modes[rgb_mode_base].on == new_state) {
		return 0;
	}
	if (new_state) {
		rgb_modes[rgb_mode_base].on = *prev_state;
		*prev_state = false;
		return rgb_backlight_on();
	} else {
		rgb_modes[rgb_mode_base].on = false;
		*prev_state = true;
		return rgb_backlight_off();
	}
}
#endif

//---------------------------------------------Listener---------------------------------------------

static int rgb_backlight_event_listener(const zmk_event_t *eh) {
	if (as_zmk_position_state_changed(eh)) {
		rgb_backlight_keypress_lightup_event_handler(eh);
		rgb_backlight_keypress_ripple_event_handler(eh);
		return 0;
	}

#if IS_ENABLED(CONFIG_RGB_BACKLIGHT_LAYERS) && IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
	if (as_zmk_layer_state_changed(eh)) {
		uint8_t index = zmk_keymap_highest_layer_active();
		rgb_backlight_layer_color_event_handler(index);
		return 0;
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

//------------------------------------------Subscriptions-------------------------------------------

ZMK_SUBSCRIPTION(rgb_backlight, zmk_position_state_changed);

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_IDLE)
ZMK_SUBSCRIPTION(rgb_backlight, zmk_activity_state_changed);
#endif

#if IS_ENABLED(CONFIG_RGB_BACKLIGHT_LAYERS) && IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
ZMK_SUBSCRIPTION(rgb_backlight, zmk_layer_state_changed);
#endif

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB)
ZMK_SUBSCRIPTION(rgb_backlight, zmk_usb_conn_state_changed);
#endif
