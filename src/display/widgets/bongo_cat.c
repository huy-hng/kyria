#include "zephyr/sys_clock.h"
#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display/widgets/bongo_cat.h>

// TODO: consider using zephyrs stack api
// https://docs.zephyrproject.org/latest/doxygen/html/group__stack__apis.html

#define TAPPING_DELAY 10
#define PAWS_UP_DELAY 20
#define IDLE_DELAY 1000
#define IDLE_TAP_DELAY 10

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

const void **images;
uint32_t last_keycode;
int counter = 0;

// static lv_timer_t *idle_timer;
// static lv_timer_t *tapping_timer;
// static lv_timer_t *paws_up_timer;

// static struct k_timer *idle_timer;
// static struct k_timer *tapping_timer;
// static struct k_timer *paws_up_timer;

const lv_img_dsc_t *next_paw_frame;
struct zmk_widget_bongo_cat *cat;

const void *idle_images[] = {
    &idle_img1, &idle_img2, &idle_img3, &idle_img4, &idle_img5,
};

//-----------------------------------------Helper Functions-----------------------------------------

void set_img_src(void *var, int val) {
    lv_obj_t *img = (lv_obj_t *)var;
    lv_img_set_src(img, images[val]);
}

void set_img(const lv_img_dsc_t *img) {
    lv_img_set_src(cat->obj, img);
    counter++;
    k_sem_give(&sem);
}

void set_img_async(const lv_img_dsc_t *img) {
    lv_async_call((void (*)(void *))set_img, (void *)img);
}

void set_timed_img(lv_timer_t *timer) { set_img(timer->user_data); }

void set_timer(struct k_timer *timer, int delay, void *args) {
    k_timer_start(timer, K_MSEC(delay), K_FOREVER);
    if (args != NULL)
        k_timer_user_data_set(timer, args);
}

//---------------------------------------Animation Functions----------------------------------------

void play_idle_animation() {
    // k_sem_reset(&sem);

    // if (current_anim_state == anim_state_idle)
    //     return;

    current_anim_state = anim_state_idle;
    LOG_INF("----------------Idle Animation----------------");
    images = idle_images;
    lv_anim_init(&cat->anim);
    // lv_anim_set_early_apply(&cat->anim, false);
    // lv_anim_set_delay(&cat->anim, IDLE_DELAY);
    lv_anim_set_var(&cat->anim, cat->obj);
    lv_anim_set_time(&cat->anim, 800);
    lv_anim_set_values(&cat->anim, 0, 4);
    lv_anim_set_exec_cb(&cat->anim, set_img_src);
    lv_anim_set_repeat_count(&cat->anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&cat->anim, 100);
    lv_anim_start(&cat->anim);
}

void lift_paws() {
    // if (current_anim_state == anim_state_tapping)
    if (current_anim_state == anim_state_paws_up) {
        LOG_INF("----------------Paws already lifted----------------");
        return;
    }
    LOG_INF("----------------Lifting Paws----------------");

    current_anim_state = anim_state_paws_up;

    // set_img(&paws_up_img);
    set_img(&paws_up_img);
}

void switch_paws() {
    next_paw_frame = next_paw_frame == &left_paw_img ? &right_paw_img : &left_paw_img;
}

void hit_table() {
    LOG_INF("----------------Hitting Table----------------");
    set_img(next_paw_frame);
    current_anim_state = anim_state_tapping;
}

void cancel_idle_animation() {
    lv_anim_del(cat->obj, set_img_src);
    lift_paws();
}

K_TIMER_DEFINE(tapping_timer, hit_table, NULL);
K_TIMER_DEFINE(idle_timer, play_idle_animation, cancel_idle_animation);
K_TIMER_DEFINE(paws_up_timer, lift_paws, NULL);

void set_bongo_cat_img(const struct zmk_keycode_state_changed *ev) {
    // void set_bongo_cat_img() {
    //     const struct zmk_keycode_state_changed *ev = k_timer_user_data_get(&update_timer);
    lv_anim_del(cat->obj, set_img_src);
    k_timer_stop(&idle_timer);
    // lv_timer_set_repeat_count(paws_up_timer, 0);

    // if (strcmp(paws_up_timer->user_data, "paws up")) {
    //     char text[20];
    //     snprintf(text, sizeof(text), "%s", paws_up_timer->user_data);
    //     lv_label_set_text(cat->debug, text);
    // }

    if (ev->state) {
        // k_timer_stop(&paws_up_timer);
        if (ev->keycode != last_keycode)
            switch_paws();
        // else if (current_anim_state == anim_state_tapping)
        //     lift_paws();

        int delay = TAPPING_DELAY;

        // if (current_anim_state == anim_state_idle) {
        //     lift_paws();
        //     delay = IDLE_TAP_DELAY;
        // }

        int err = k_sem_take(&sem, K_MSEC(2));
        if (!err) {
            set_timer(&tapping_timer, delay, NULL);
        }
        last_keycode = ev->keycode;

    } else {
        // k_timer_stop(&tapping_timer);
        // k_timer_stop(&tapping_timer);

        int err = k_sem_take(&sem, K_MSEC(2));
        if (!err) {
            set_timer(&paws_up_timer, PAWS_UP_DELAY, NULL);
        }
        // set_timer(&idle_timer, IDLE_DELAY, NULL);
        // lv_async_call(lift_paws, NULL);
    }
}

//------------------------------------------ZMK Functions-------------------------------------------
void print() {
    char text[20];
    snprintf(text, sizeof(text), "%d %d", k_sem_count_get(&sem), counter);
    lv_label_set_text(cat->debug, text);
}

K_TIMER_DEFINE(sem_timer, print, NULL);

int bongo_cat_update_cb(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
    set_bongo_cat_img(ev);
    // k_timer_start(&update_timer, K_NO_WAIT, K_FOREVER);
    // k_timer_user_data_set(&update_timer, &ev);
    // lv_async_call((void (*)(void *))set_bongo_cat_img, (void *)ev);
    return ZMK_EV_EVENT_BUBBLE;
}

int zmk_widget_bongo_cat_init(struct zmk_widget_bongo_cat *widget, lv_obj_t *parent) {
    widget->obj = lv_img_create(parent);
    widget->debug = lv_label_create(parent);
    lv_obj_align(widget->debug, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    cat = widget;

    lv_label_set_text(widget->debug, " ");
    k_timer_start(&sem_timer, K_SECONDS(1), K_MSEC(50));

    play_idle_animation();
    // k_timer_init(idle_timer, play_idle_animation, NULL);

    // k_timer_init(paws_up_timer, lift_paws, lift_paws);
    // k_timer_start(paws_up_timer, K_MSEC(500), K_SECONDS(4));

    lv_img_cache_set_size(8);

    // idle_timer = set_timer(play_idle_animation, IDLE_DELAY, "idle");
    // lv_timer_set_repeat_count(idle_timer, -1);
    // lv_timer_ready(idle_timer);

    sys_slist_append(&widgets, &widget->node);
    return 0;
}

lv_obj_t *zmk_widget_bongo_cat_obj(struct zmk_widget_bongo_cat *widget) { return widget->obj; }

ZMK_LISTENER(zmk_widget_bongo_cat, bongo_cat_update_cb)
ZMK_SUBSCRIPTION(zmk_widget_bongo_cat, zmk_keycode_state_changed);

#include <zephyr/init.h>

// SYS_INIT(zmk_widget_bongo_cat_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
