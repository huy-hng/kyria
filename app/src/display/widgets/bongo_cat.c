#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "display/widgets/bongo_cat.h"

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

// TODO: consider using zephyrs stack api
// https://docs.zephyrproject.org/latest/doxygen/html/group__stack__apis.html

#define USE_ABSOLUTE_PAW 0

#define DOUBLE_TAP_DELAY 50
#define IDLE_TAP_DELAY 100
#define PAWS_UP_DELAY 150
#define IDLE_DELAY 1000

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

enum anim_state {
	anim_state_idle,
	anim_state_paws_up,
	anim_state_tapping,
} current_anim_state;

LV_IMG_DECLARE(idle_img1);
LV_IMG_DECLARE(idle_img2);
LV_IMG_DECLARE(idle_img3);
LV_IMG_DECLARE(idle_img4);
LV_IMG_DECLARE(idle_img5);
LV_IMG_DECLARE(paws_up_img);
LV_IMG_DECLARE(right_paw_img);
LV_IMG_DECLARE(left_paw_img);

K_SEM_DEFINE(sem, 1, 1);

K_TIMER_DEFINE(idle_timer, start_idle_animation, NULL);
K_TIMER_DEFINE(paws_up_timer, lift_paws, NULL);
K_TIMER_DEFINE(tapping_timer, hit_table, NULL);

const void **images;
uint32_t last_keycode;

const lv_img_dsc_t *next_paw_frame;
struct zmk_widget_bongo_cat *CAT;

const void *idle_images[] = {
	&idle_img1, &idle_img2, &idle_img3, &idle_img4, &idle_img5,
};

//-----------------------------------------Helper Functions-----------------------------------------

void set_img_src(void *var, int val) {
	lv_obj_t *img = (lv_obj_t *)var;
	lv_img_set_src(img, images[val]);
}
void set_img(const lv_img_dsc_t *img) { lv_img_set_src(CAT->obj, img); }
void _set_paw_img(const lv_img_dsc_t *img) { lv_img_set_src(CAT->tapping, img); }
void set_paw_img(const lv_img_dsc_t *img) { lv_async_call((void (*)())_set_paw_img, (void *)img); }

void set_timer(struct k_timer *timer, int delay, void *args) {
	k_timer_start(timer, K_MSEC(delay), K_FOREVER);
	if (args != NULL)
		k_timer_user_data_set(timer, args);
}

//---------------------------------------Animation Functions----------------------------------------

void show_widget(int show) {
	int x = show ? 0 : -200;
	int err = k_sem_take(&sem, K_MSEC(20));
	if (!err)
		lv_obj_set_x(CAT->tapping, x);
	k_sem_give(&sem);
}

void start_idle_animation() {
	if (current_anim_state == anim_state_idle)
		return;

	LOG_DBG("----------------Idle Animation----------------");
	current_anim_state = anim_state_idle;
	images = idle_images;
	lv_anim_init(&CAT->anim);
	lv_anim_set_var(&CAT->anim, CAT->obj);
	lv_anim_set_time(&CAT->anim, 800);
	lv_anim_set_values(&CAT->anim, 0, 4);
	lv_anim_set_exec_cb(&CAT->anim, set_img_src);
	lv_anim_set_repeat_count(&CAT->anim, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_repeat_delay(&CAT->anim, 200);
	lv_anim_start(&CAT->anim);
}

void change_state_paws_up() { current_anim_state = anim_state_paws_up; }

void lift_paws() {
	LOG_DBG("----------------Lifting Paws----------------");
	if (current_anim_state == anim_state_tapping)
		show_widget(0);

	current_anim_state = anim_state_paws_up;
	// k_timer_stop(&state_paws_up_timer);
	// set_timer(&state_paws_up_timer, DOUBLE_TAP_DELAY, NULL);
}

void hit_table() {
	LOG_DBG("----------------Hitting Table----------------");
	if (current_anim_state != anim_state_tapping)
		show_widget(1);
	current_anim_state = anim_state_tapping;
}

void switch_paws() {
	next_paw_frame = next_paw_frame == &left_paw_img ? &right_paw_img : &left_paw_img;
	set_paw_img(next_paw_frame);
}

void key_release() {
	set_timer(&paws_up_timer, PAWS_UP_DELAY, NULL);
	set_timer(&idle_timer, IDLE_DELAY, NULL);
}

void key_press(uint32_t keycode) {
	int delay = 0;

	if (keycode != last_keycode)
		switch_paws();

	if (current_anim_state == anim_state_idle)
		delay = IDLE_TAP_DELAY;
	else if (current_anim_state == anim_state_tapping && keycode == last_keycode) {
		lift_paws();
		delay = DOUBLE_TAP_DELAY;
	}

	if (delay > 0)
		set_timer(&tapping_timer, delay, NULL);
	else
		hit_table();
}

void set_bongo_cat_img(const struct zmk_position_state_changed *ev) {
	lv_anim_del(CAT->obj, set_img_src);
	k_timer_stop(&idle_timer);
	k_timer_stop(&paws_up_timer);
	// k_timer_stop(&tapping_timer);

	if (current_anim_state == anim_state_idle)
		set_img(&paws_up_img);

	if (ev->state)
		key_press(ev->position);
	else
		key_release();

	last_keycode = ev->position;
}

//------------------------------------------ZMK Functions-------------------------------------------

int bongo_cat_update_cb(const zmk_event_t *eh) {
	const struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);

	struct zmk_widget_bongo_cat *widget;
	SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
		CAT = widget;
		set_bongo_cat_img(ev);
	}
	return ZMK_EV_EVENT_BUBBLE;
}

lv_obj_t *zmk_widget_bongo_cat_init(struct zmk_widget_bongo_cat *widget, lv_obj_t *parent) {
	lv_img_cache_set_size(8);

	CAT = widget;
	widget->obj = lv_img_create(parent);
	widget->tapping = lv_img_create(widget->obj);

	current_anim_state = anim_state_tapping;
	set_paw_img(&right_paw_img);
	start_idle_animation();

	show_widget(0);

	sys_slist_append(&widgets, &widget->node);
	return widget->obj;
}

lv_obj_t *zmk_widget_bongo_cat_obj(struct zmk_widget_bongo_cat *widget) { return widget->obj; }

ZMK_LISTENER(zmk_widget_bongo_cat, bongo_cat_update_cb)
ZMK_SUBSCRIPTION(zmk_widget_bongo_cat, zmk_position_state_changed);
