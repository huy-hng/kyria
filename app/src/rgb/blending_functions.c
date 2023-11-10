#include "rgb/rgb_backlight.h"

struct rgb_backlight_blending_fn blending_fns;

// clang-format off
static float             add(float val1, float val2, float ratio) { return val1 + val2; }
static float            mask(float val1, float val2, float ratio) { return val1 * ratio; }
static float         replace(float val1, float val2, float ratio) { return val2; }
static float weighted_interp(float val1, float val2, float ratio) {
	return sqrtf(SQUARE(val1) * (1 - ratio) + SQUARE(val2) * ratio);
}
static float linear_interp(float val1, float val2, float ratio) {
	return val1 + ((val2 - val1) * ratio);
}
// clang-format on

void rgb_backlight_initialize_blending_fns() {
	blending_fns = (struct rgb_backlight_blending_fn){
		.add = add,
		.mask = mask,
		.replace = replace,
		.linear_interp = linear_interp,
		.weighted_interp = weighted_interp,
	};
}
