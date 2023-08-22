#include <drivers/behavior.h>
#include <zmk/ble.h>
#include <zmk/split/bluetooth/central.h>
#include "utils.h"

bool same_str(const char *str1, const char *str2) { //
	return strcmp(str1, str2) == 0;
}

struct zmk_behavior_binding_event event = {.layer = 0, .position = 0, .timestamp = 0};

void send_to_peripheral(char *behavior, int param1, int param2) {
	struct zmk_behavior_binding binding = {
		.behavior_dev = behavior,
		.param1 = param1,
		.param2 = param2,
	};

	for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
		zmk_split_bt_invoke_behavior(i, &binding, event, true);
	}
}

void invoke_behavior(char *behavior, int param1, int param2) {
	struct zmk_behavior_binding binding = {
		.behavior_dev = behavior,
		.param1 = param1,
		.param2 = param2,
	};

	behavior_keymap_binding_pressed(&binding, event);
}

void invoke_behavior_global(char *behavior, int param1, int param2) {
	send_to_peripheral(behavior, param1, param2);
	invoke_behavior(behavior, param1, param2);
	
}
