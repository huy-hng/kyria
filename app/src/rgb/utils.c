#include "rgb_backlight.h"

struct zmk_led_hsb hsb_scale_min_max(struct zmk_led_hsb hsb) {
	hsb.b = CONFIG_ZMK_RGB_UNDERGLOW_BRT_MIN +
			(CONFIG_ZMK_RGB_UNDERGLOW_BRT_MAX - CONFIG_ZMK_RGB_UNDERGLOW_BRT_MIN) * hsb.b / BRT_MAX;
	return hsb;
}

struct zmk_led_hsb hsb_scale_zero_max(struct zmk_led_hsb hsb) {
	hsb.b = hsb.b * CONFIG_ZMK_RGB_UNDERGLOW_BRT_MAX / BRT_MAX;
	return hsb;
}

struct led_rgb hsb_to_rgb(struct zmk_led_hsb hsb) {
	float r, g, b;

	uint8_t i = hsb.h / 60;
	float v = hsb.b / ((float)BRT_MAX);
	float s = hsb.s / ((float)SAT_MAX);
	float f = hsb.h / ((float)HUE_MAX) * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (i % 6) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
		r = v;
		g = p;
		b = q;
		break;
	}

	struct led_rgb rgb = {
		.r = r * 255,
		.g = g * 255,
		.b = b * 255,
	};

	return rgb;
}

void convert_hsb_to_rgb(struct zmk_led_hsb *pixels, struct led_rgb *converted){
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		converted[i] = hsb_to_rgb(hsb_scale_min_max(pixels[i]));
	}
}

