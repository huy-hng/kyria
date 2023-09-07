#include <dt-bindings/zmk/rgb.h>

// #define RGB_TOG_CMD 0
// #define RGB_ON_CMD 1
// #define RGB_OFF_CMD 2
// #define RGB_HUI_CMD 3
// #define RGB_HUD_CMD 4
// #define RGB_SAI_CMD 5
// #define RGB_SAD_CMD 6
// #define RGB_BRI_CMD 7
// #define RGB_BRD_CMD 8
// #define RGB_SPI_CMD 9
// #define RGB_SPD_CMD 10
// #define RGB_EFF_CMD 11
// #define RGB_EFR_CMD 12
// #define RGB_EFS_CMD 13
// #define RGB_COLOR_HSB_CMD 14

// #define RGB_TOG RGB_TOG_CMD 0
// #define RGB_ON RGB_ON_CMD 0
// #define RGB_OFF RGB_OFF_CMD 0
// #define RGB_HUI RGB_HUI_CMD 0
// #define RGB_HUD RGB_HUD_CMD 0
// #define RGB_SAI RGB_SAI_CMD 0
// #define RGB_SAD RGB_SAD_CMD 0
// #define RGB_BRI RGB_BRI_CMD 0
// #define RGB_BRD RGB_BRD_CMD 0
// #define RGB_SPI RGB_SPI_CMD 0
// #define RGB_SPD RGB_SPD_CMD 0
// #define RGB_EFF RGB_EFF_CMD 0
// #define RGB_EFR RGB_EFR_CMD 0
// #define RGB_COLOR_HSB_VAL(h, s, v) (((h) << 16) + ((s) << 8) + (v))
// #define RGB_COLOR_HSB(h, s, v) RGB_COLOR_HSB_CMD##(RGB_COLOR_HSB_VAL(h, s, v))
// #define RGB_COLOR_HSV RGB_COLOR_HSB

#define RGB_SET_HUE 15
#define RGB_SET_SAT 16
#define RGB_SET_BRT 17

#define RGB_SET_EFFECT_MODE 18
#define RGB_SET_HSBA_CMD 19
// #define RGB_SET_HSBA_LAYERS_CMD 19

#define RGB_EFS_UDG 20
#define RGB_SET_UDG_HSB_CMD 21

#define RGB_ENCODE_BEHAVIOR(behavior, data) ((behavior) + ((data) << 8))
// #define RGB_DECODE_BEHAVIOR(behavior, mode) ((behavior) + ((mode) << 8))

#define RGB_ENCODE_HSBA(h, s, v, a) (((h) << 21) + ((s) << 14) + ((v) << 7) + (a))
#define RGB_DECODE_HSBA(ENCODED)       \
	(struct led_hsba){                 \
		.h = (ENCODED >> 21) & 0xFFFF, \
		.s = (ENCODED >> 14) & 0x7F,   \
		.b = (ENCODED >> 7) & 0x7F,    \
		.a = ENCODED & 0x7F            \
	}

#define RGB_DECODE_HSB(ENCODED)        \
	(struct led_hsb){                  \
		.h = (ENCODED >> 16) & 0xFFFF, \
		.s = (ENCODED >> 8) & 0xFF,    \
		.b = ENCODED & 0xFF            \
	}
