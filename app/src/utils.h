#include <string.h>

#include <zephyr/init.h>
#include <drivers/behavior.h>

#include <zmk/ble.h>
#include <zmk/split/bluetooth/central.h>

#define RGB_UG "RGB_UG"

bool same_str(const char *str1, const char *str2);

int invoke_behavior_global(char *behavior, int param1, int param2);
int send_to_peripheral(char *behavior, int param1, int param2);
int invoke_behavior(char *behavior, int param1, int param2);
int _send_to_peripheral(struct zmk_behavior_binding *binding,
						struct zmk_behavior_binding_event event);
