#include "display/widgets/menu.h"

void style_roller(lv_obj_t *roller) {
	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_anim_time(&style, 300);
	lv_style_set_border_width(&style, 0);
	lv_style_set_pad_hor(&style, 2);
	// lv_style_set_border_side(&style, LV_BORDER_SIDE_TOP);
	lv_style_set_text_letter_space(&style, -1);

	// lv_style_set_outline_width(&style, 2);
	// lv_style_set_outline_pad(&style, 5);
	lv_obj_add_style(roller, &style, 0);

	// lv_obj_set_size(roller1, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
	// lv_obj_set_height(roller1, lv_disp_get_ver_res(NULL));
	lv_roller_set_visible_row_count(roller, 3);
}

void create_roller(struct component_obj *roller) {
	roller->obj = lv_roller_create(roller->container);
	style_roller(roller->obj);

	lv_obj_align(roller->obj, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_style_text_font(roller->obj, &lv_font_unscii_8, LV_PART_MAIN);
	// lv_coord_t height = lv_obj_get_height(widget->obj);

	roller->label = create_text(roller->container, "Menu");
	lv_obj_t *line = create_line(roller->container);

	lv_obj_align(roller->label, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_align_to(line, roller->obj, LV_ALIGN_OUT_TOP_MID, 2, 2);

	initialize_group(roller, true);
}
