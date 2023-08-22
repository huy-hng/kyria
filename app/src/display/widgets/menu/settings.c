#include "../headers/menu.h"

static void back_event_handler(lv_event_t *e) {
	lv_obj_t *obj = lv_event_get_target(e);
	lv_obj_t *menu = lv_event_get_user_data(e);

	if (lv_menu_back_btn_is_root(menu, obj)) {
		// lv_obj_t * mbox1 = lv_msgbox_create(NULL, "Hello", "Root back btn click.", NULL, true);
		// lv_obj_center(mbox1);
	}
}

void create_settings(struct component_obj *settings) {
	settings->obj = lv_menu_create(settings->container);
	lv_obj_set_style_text_font(settings->obj, &lv_font_montserrat_10, LV_PART_MAIN);
	lv_menu_set_mode_root_back_btn(settings->obj, LV_MENU_ROOT_BACK_BTN_ENABLED);
	lv_obj_add_event_cb(settings->obj, back_event_handler, LV_EVENT_CLICKED, settings->obj);
	lv_obj_set_size(settings->obj, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
	lv_obj_center(settings->obj);

	lv_obj_t *cont;
	lv_obj_t *label;

	/*Create a sub page*/
	lv_obj_t *sub_page = lv_menu_page_create(settings->obj, NULL);

	cont = lv_menu_cont_create(sub_page);
	label = lv_label_create(cont);
	lv_label_set_text(label, "Hello, I am hiding here");

	settings->group = lv_group_create();
	lv_group_add_obj(settings->group, settings->obj);

	/*Create a main page*/
	lv_obj_t *main_page = lv_menu_page_create(settings->obj, NULL);

	cont = lv_menu_cont_create(main_page);
	label = lv_label_create(cont);
	lv_label_set_text(label, "Item 1");
	lv_group_add_obj(settings->group, label);

	cont = lv_menu_cont_create(main_page);
	label = lv_label_create(cont);
	lv_label_set_text(label, "Item 2");
	lv_group_add_obj(settings->group, label);

	cont = lv_menu_cont_create(main_page);
	label = lv_label_create(cont);
	lv_label_set_text(label, "Item 3 (Click me!)");

	lv_menu_set_load_page_event(settings->obj, cont, sub_page);
	lv_menu_set_page(settings->obj, main_page);

	lv_group_add_obj(settings->group, label);


	// initialize_group(settings, false);
}

void show_menu_settings() {
	hide_component(roller);
	show_component(settings);
}
