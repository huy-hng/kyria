#include <string.h>

#include <zephyr/init.h>
#include <drivers/behavior.h>

#include <zmk/ble.h>
#include <zmk/split/bluetooth/central.h>

#define RGB_UG "RGB_UG"

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;

void queue_lowprio_work(struct k_timer *timer);

char *format_text(char *fmt, ...);
bool same_str(const char *str1, const char *str2);

int invoke_behavior_global(char *behavior, int param1, int param2);
int send_to_peripheral(char *behavior, int param1, int param2);
int invoke_behavior(char *behavior, int param1, int param2);
int _send_to_peripheral(struct zmk_behavior_binding *binding,
						struct zmk_behavior_binding_event event);

// debug_set_text_fmt("%d %d %d", color.h, color.s, color.b);

// uint hue = color.h;
// debug_newline_text_fmt("%d", (ulong)hue);

// uint hue0 = hue;
// uint hue1 = hue << 9;
// uint hue2 = hue << 18;
// uint hue3 = hue << 27;
// uint hue4 = hue << 32;

// ullong combined = hue1 + hue2 + hue3;
// debug_append_text_fmt("%d", (combined >> 32) & 0xFFFF);
// debug_newline_text_fmt("%d", (uint)((combined >>  9) & 0x1FF));
// debug_newline_text_fmt("%d", (uint)((combined >> 18) & 0x1FF));
// debug_newline_text_fmt("%d", (uint)((combined >> 27) & 0x1FF));
// debug_newline_text_fmt("%d", (uint)((combined >> 36)));
