#include "rgb_backlight.h"
#include "../imports.h"

// clang-format off
int left_pixels[25] = {
	30, 29, 28, 27, 26, 25,
	24, 23, 22, 21, 20, 19,
	18, 17, 16, 15, 14, 13, 12, 11,
				10,  9,  8,  7,  6
};
int left_underglow[6] = {
	2, 1, 0,
	3, 4, 5
};
int right_pixels[25] = {
			25, 26, 27, 28, 29, 30,
			19, 20, 21, 22, 23, 24,
	11, 12, 13, 14, 15, 16, 17, 18,
	 6,  7,  8,  9, 10
};
int right_underglow[6] = {
	0, 1, 2,
	5, 4, 3
};
// clang-format on

// first value is num of pixels because i dont know how to code
int i_0[2] = {1, 25};
int i_1[3] = {2, 26, 27};
int i_2[2] = {1, 29};
int i_3[3] = {2, 10, 17};
int i_4[3] = {2, 8, 9};
int i_5[3] = {2, 6, 11};
int *right_underglow_lookup[6] = {i_0 + 1, i_1 + 1, i_2 + 1, i_3 + 1, i_4 + 1, i_5 + 1};

struct led_rgb_float average_pixels(rgb_strip_float pixels, int underglow_index) {
	int *indices = right_underglow_lookup[underglow_index];
	int arr_len = indices[-1];

	float r = 0;
	float g = 0;
	float b = 0;
	struct led_rgb_float pixel;

	for (int i = 0; i < arr_len; i++) {
		pixel = pixels[indices[i]];
		r += pixel.r * pixel.r;
		g += pixel.g * pixel.g;
		b += pixel.b * pixel.b;
	}

	// sqrt(R1 ^ 2 * w + R2 ^ 2 * [1 - w]);
	// sqrt(G1 ^ 2 * w + G2 ^ 2 * [1 - w]);
	// sqrt(B1 ^ 2 * w + B2 ^ 2 * [1 - w]);
	return (struct led_rgb_float){
		.r = sqrtf(r / arr_len) + 1.0 / 255,
		.g = sqrtf(g / arr_len) + 1.0 / 255,
		.b = sqrtf(b / arr_len) + 1.0 / 255,
	};
}

void copy_overglow(rgb_strip_float pixels) {
	for (int i = 0; i < 6; i++) {
		pixels[i] = average_pixels(pixels, i);
	}
}

void rgb_underglow_animation_set_pixels(int effect, rgb_strip_float pixels) {
	switch (effect) {
	case RGB_UNDERGLOW_ANIMATION_OFF:
		rgb_backlight_effect_off(pixels, UNDERGLOW_INDEX_START, UNDERGLOW_NUM_PIXELS);
		break;
	case RGB_UNDERGLOW_ANIMATION_COPY:
		copy_overglow(pixels);
		break;
	case RGB_UNDERGLOW_ANIMATION_SOLID: {
		int before = rgb_states.base.color.b;
		rgb_states.base.color.b = before * 1.3;
		rgb_backlight_animation_solid(pixels, UNDERGLOW_INDEX_START, UNDERGLOW_NUM_PIXELS);
		rgb_states.base.color.b = before;
		break;
	}
	}
}
