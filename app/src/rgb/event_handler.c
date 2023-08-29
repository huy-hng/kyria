#include <zmk/events/activity_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/events/layer_state_changed.h>

#include "rgb_backlight.h"

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
#endif // IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_IDLE) ||
	   // IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB)

//---------------------------------------------Listener---------------------------------------------

static int rgb_backlight_event_listener(const zmk_event_t *eh) {

#if IS_ENABLED(CONFIG_RGB_BACKLIGHT_LAYERS) && IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
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

//------------------------------------------Subscriptions-------------------------------------------

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_IDLE)
ZMK_SUBSCRIPTION(rgb_backlight, zmk_activity_state_changed);
#endif

#if IS_ENABLED(CONFIG_RGB_BACKLIGHT_LAYERS) && IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
ZMK_SUBSCRIPTION(rgb_backlight, zmk_layer_state_changed);
#endif

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB)
ZMK_SUBSCRIPTION(rgb_backlight, zmk_usb_conn_state_changed);
#endif
