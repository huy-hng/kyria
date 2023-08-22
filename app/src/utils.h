#include <string.h>
#define RGB_UG "RGB_UG"

bool same_str(const char *str1, const char *str2);

void invoke_behavior_global(char *behavior, int param1, int param2);
void send_to_peripheral(char *behavior, int param1, int param2);
void invoke_behavior(char *behavior, int param1, int param2);
