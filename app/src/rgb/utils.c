#include "rgb/rgb_backlight.h"

// deprecated
void copy_pixel_array(rgba_strip arr1, rgba_strip arr2) {
	for (int i = 0; i < STRIP_NUM_PIXELS; i++)
		arr2[i] = arr1[i];
}

// return a led_rgba pixel with input sanitized
struct led_rgba create_rgba_pixel(float r, float g, float b, float alpha) {
	return (struct led_rgba){
		.r = CLAMP(r, 0, 1),
		.g = CLAMP(g, 0, 1),
		.b = CLAMP(b, 0, 1),
		.a = CLAMP(alpha, 0, 1),
	};
}

void set_pixel(struct led_rgba *pixel, float r, float g, float b, float a) {
	pixel->r = CLAMP(r, 0, 1);
	pixel->g = CLAMP(g, 0, 1);
	pixel->b = CLAMP(b, 0, 1);
	pixel->a = 1;
	return;
}

void set_pixel_white(struct led_rgba *pixel, float value, float alpha) { //
	set_pixel(pixel, value, value, value, alpha);
}

// unnecessary functions?
void change_pixel(struct led_rgba *pixel, float r, float g, float b, float a) {
	set_pixel(pixel, pixel->r + r, pixel->g + g, pixel->b + b, a);
}

void change_pixel_white(struct led_rgba *pixel, float value, float alpha) {
	change_pixel(pixel, value, value, value, alpha);
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

struct led_rgba create_rgb(float red, float green, float blue) {
	static float r, g, b;
	return (struct led_rgba){.r = r, .g = g, .b = b};
}

struct led_rgba hsb_to_rgb(struct led_hsb hsb) {
	static struct led_rgba output;

	uint8_t i = hsb.h / 60;
	float v = hsb.b / ((float)BRT_MAX);
	float s = hsb.s / ((float)SAT_MAX);
	float f = hsb.h / ((float)HUE_MAX) * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (i % 6) {
	case 0:
		output.r = v;
		output.g = t;
		output.b = p;
		break;
	case 1:
		output.r = q;
		output.g = v;
		output.b = p;
		break;
	case 2:
		output.r = p;
		output.g = v;
		output.b = t;
		break;
	case 3:
		output.r = p;
		output.g = q;
		output.b = v;
		break;
	case 4:
		output.r = t;
		output.g = p;
		output.b = v;
		break;
	case 5:
		output.r = v;
		output.g = p;
		output.b = q;
		break;
	}
	output.a = hsb.a / 100.0;
	return output;
}
