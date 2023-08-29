#include "utils.h"

bool same_str(const char *str1, const char *str2) { //
	return strcmp(str1, str2) == 0;
}

int get_array_index(int *arr[], void *elem) {
	int index = -1;
	int arrLen = sizeof(*arr) / sizeof(arr[0]);

	for (int i = 0; i < arrLen; i++) {
		if (arr[i] == elem) {
			index = i;
			break;
		}
	}

	return index;
}

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
struct zmk_behavior_binding_event event = {.layer = 0, .position = 0, .timestamp = 0};
int _send_to_peripheral(struct zmk_behavior_binding *binding,
						struct zmk_behavior_binding_event event) {
	int err;
	for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
		err = zmk_split_bt_invoke_behavior(i, binding, event, true);
	}
	return err;
}

int send_to_peripheral(char *behavior, int param1, int param2) {
	struct zmk_behavior_binding binding = {
		.behavior_dev = behavior,
		.param1 = param1,
		.param2 = param2,
	};

	return _send_to_peripheral(&binding, event);
}

int invoke_behavior(char *behavior, int param1, int param2) {
	struct zmk_behavior_binding binding = {
		.behavior_dev = behavior,
		.param1 = param1,
		.param2 = param2,
	};

	return behavior_keymap_binding_pressed(&binding, event);
}

int invoke_behavior_global(char *behavior, int param1, int param2) {
	send_to_peripheral(behavior, param1, param2);
	return invoke_behavior(behavior, param1, param2);
}
#endif
