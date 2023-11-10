#include <zmk/workqueue.h>
#include "utils.h"
#include "imports.h"

void queue_lowprio_work(struct k_timer *timer) {
	k_work_submit_to_queue(zmk_workqueue_lowprio_work_q(), k_timer_user_data_get(timer));
}

char *format_text(char *fmt, ...) {
	static char new_text[100];

	va_list args;
	va_start(args, fmt);

	vsprintf(new_text, fmt, args);
	return new_text;
}

bool same_str(const char *str1, const char *str2) { //
	return strcmp(str1, str2) == 0;
}

long power(int x, unsigned n) {
	// Initialize result to 1
	long long pow = 1;

	// Multiply x for n times
	for (int i = 0; i < n; i++) {
		pow = pow * x;
	}

	return pow;
}

// Reverses a string 'str' of length 'len'
void reverse(char *str, int len) {
	int i = 0, j = len - 1, temp;
	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d) {
	int i = 0;
	while (x) {
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}

	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
		str[i++] = '0';

	reverse(str, i);
	str[i] = '\0';
	return i;
}

// Converts a floating-point/double number to a string.
// returns a static string that needs to be used before recalling this function
char *ftos(float n, int decimal_places) {
	static char res[10];

	// Extract integer part
	int ipart = (int)n;

	// Extract floating part
	float fpart = n - (float)ipart;

	// convert integer part to string
	int i = intToStr(ipart, res, 1);

	// check for display option after point
	if (decimal_places != 0) {
		res[i] = '.'; // add dot

		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter
		// is needed to handle cases like 233.007
		fpart = fpart * power(10, decimal_places);

		intToStr((int)fpart, res + i + 1, decimal_places);
	}

	return res;
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
