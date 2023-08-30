#include "rgb_backlight.h"

void copy_pixel_array(rgb_strip_float arr1, rgb_strip_float arr2) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++)
		arr2[i] = arr1[i];
}

void set_pixel(struct led_rgb_float *pixel, float r, float g, float b, bool absolute) {
	if (absolute) {
		pixel->r = CLAMP(r, 0, 1);
		pixel->g = CLAMP(g, 0, 1);
		pixel->b = CLAMP(b, 0, 1);
		return;
	}

	pixel->r = CLAMP(pixel->r + r, 0, 1);
	pixel->g = CLAMP(pixel->g + g, 0, 1);
	pixel->b = CLAMP(pixel->b + b, 0, 1);
}

void set_pixel_white(struct led_rgb_float *pixel, float value, bool absolute) {
	set_pixel(pixel, value, value, value, absolute);
}

struct led_hsb hsb_scale_min_max(struct led_hsb hsb) {
	hsb.b = CONFIG_ZMK_RGB_UNDERGLOW_BRT_MIN +
			(CONFIG_ZMK_RGB_UNDERGLOW_BRT_MAX - CONFIG_ZMK_RGB_UNDERGLOW_BRT_MIN) * hsb.b / BRT_MAX;
	return hsb;
}

struct led_hsb hsb_scale_zero_max(struct led_hsb hsb) {
	hsb.b = hsb.b * CONFIG_ZMK_RGB_UNDERGLOW_BRT_MAX / BRT_MAX;
	return hsb;
}

struct led_rgb_float create_rgb(float red, float green, float blue) {
	static float r, g, b;
	return (struct led_rgb_float){.r = r, .g = g, .b = b};
}

struct led_rgb_float hsb_to_rgb(struct led_hsb hsb) {
	static float r, g, b;

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

	return (struct led_rgb_float){.r = r, .g = g, .b = b};
}

void convert_hsb_to_rgb(hsb_strip pixels, rgb_strip_float converted) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		converted[i] = hsb_to_rgb(hsb_scale_min_max(pixels[i]));
	}
}

void rgb_strip_float_2_rgb_strip(rgb_strip_float rgb_float, rgb_strip rgb) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
		rgb[i].r = CLAMP(rgb_float[i].r * 255, 0, 255);
		rgb[i].g = CLAMP(rgb_float[i].g * 255, 0, 255);
		rgb[i].b = CLAMP(rgb_float[i].b * 255, 0, 255);
	}
}
