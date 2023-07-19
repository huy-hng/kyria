#include "misc/lv_timer.h"
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display/widgets/bongo_cat.h>

// #if IS_ENABLED(CONFIG_ZMK_DISPLAY_WORK_QUEUE_DEDICATED)
// #error "Bongo cat only works with the system work queue currently"
// #endif

#define ANIMATION_DEBOUNCE 2
#define IDLE_DELAY 500

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

enum anim_state {
    anim_state_none,
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


const void **images;
uint8_t images_len;
uint32_t last_keycode;
uint32_t last_key_press;
uint32_t last_key_release;
bool is_some_key_pressed;

const lv_timer_t *idle_timer;
const lv_timer_t *tapping_timer;
const lv_timer_t *paws_up_timer;

const lv_img_dsc_t *next_paw_frame = &right_paw_img;
const int idle_tap_delay = 20;
struct zmk_widget_bongo_cat *cat;

const void *idle_images[] = {
    &idle_img1, &idle_img2, &idle_img3, &idle_img4, &idle_img5,
};

//-----------------------------------------Helper Functions-----------------------------------------

void set_img_src(void *var, int val) {
    lv_obj_t *img = (lv_obj_t *)var;
    lv_img_set_src(img, images[val]);
}

void set_img(const lv_img_dsc_t *img) { lv_img_set_src(cat->obj, img); }

void set_img_async(const lv_img_dsc_t *img) {
    lv_async_call((void (*)(void *))set_img, (void *)img);
}

void set_timed_img(lv_timer_t *timer) { set_img(timer->user_data); }

lv_timer_t *set_timer(lv_timer_cb_t f, int delay, void *args) {
    lv_timer_t *timer = lv_timer_create((*f), delay, args);
    lv_timer_set_repeat_count(timer, 1);
    return timer;
}

bool run_timer_now(lv_timer_t *timer) {
    LOG_INF("period %d", tapping_timer->period);
    LOG_INF("repeat_count %d", tapping_timer->repeat_count);
    LOG_INF("last_run %d", tapping_timer->last_run);

    // FIX: this error prone code
    if (timer->repeat_count > 0 && timer->repeat_count < 10) {
        lv_timer_ready(timer);
        return true;
    } else
        return false;
}

//---------------------------------------Animation Functions----------------------------------------

void play_idle_animation(struct _lv_timer_t *timer) {
    if (current_anim_state == anim_state_idle)
        return;

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
    set_img(&paws_up_img);
}

void switch_paws() {
    next_paw_frame = next_paw_frame == &left_paw_img ? &right_paw_img : &left_paw_img;
}

void hit_table() {
    LOG_INF("----------------Hitting Table----------------");

    // if (current_anim_state == anim_state_idle) {
    //     // set_img(&paws_up_img);
    //     lift_paws();
    //     set_timer(set_timed_img, idle_tap_delay, (void *)next_paw_frame);
    // } else {
    // }
    // set_img_async(next_paw_frame);
    current_anim_state = anim_state_tapping;
    set_img(next_paw_frame);
}

void update_handler(const struct zmk_keycode_state_changed *ev) {
    lv_anim_del(cat->obj, set_img_src);
    lv_timer_pause(idle_timer);
    lv_timer_set_repeat_count(paws_up_timer, 0);

    LOG_INF("period %d", paws_up_timer->period);
    LOG_INF("repeat_count %d", paws_up_timer->repeat_count);
    LOG_INF("last_run %d", paws_up_timer->last_run);
    LOG_INF("user_data %s", paws_up_timer->user_data);

    if (ev->state) {

        if (ev->keycode != last_keycode)
            switch_paws();
        else if (current_anim_state == anim_state_tapping)
            lift_paws();

        // run_timer_now(paws_up_timer);
        // lv_async_call(hit_table, NULL);
        tapping_timer = set_timer(hit_table, 5,  "tap");
        last_keycode = ev->keycode;

    } else {
        // run_timer_now(tapping_timer);
        paws_up_timer = set_timer(lift_paws, 10, "paws up");
        // lv_async_call(lift_paws, NULL);

        lv_timer_reset(idle_timer);
        lv_timer_resume(idle_timer);
    }
}

//------------------------------------------ZMK Functions-------------------------------------------

int bongo_cat_listener(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);

    // LOG_DBG("----key pressed %d----", ev->state);
    // LOG_DBG("----key code %d----", ev->keycode);
    // LOG_DBG("----usage page %d----", ev->usage_page);
    // LOG_DBG("----implicit %d----", ev->implicit_modifiers);
    // LOG_DBG("----explicit %d----", ev->explicit_modifiers);
    // LOG_DBG("----timestamp %d----", ev->timestamp);

    lv_async_call((void (*)(void *))update_handler, (void *)ev);
    // update_handler(ev);
    return ZMK_EV_EVENT_BUBBLE;
}

int zmk_widget_bongo_cat_init(struct zmk_widget_bongo_cat *widget, lv_obj_t *parent) {
    widget->obj = lv_img_create(parent);
    cat = widget;

    lv_img_cache_set_size(8);
    // idle_timer = lv_timer_create_basic();
    // lv_timer_set_cb(idle_timer, play_idle_animation);
    // lv_timer_set_period(idle_timer, IDLE_DELAY);

    idle_timer = set_timer(play_idle_animation, IDLE_DELAY, "idle");
    lv_timer_set_repeat_count(idle_timer, -1);
    lv_timer_ready(idle_timer);

    sys_slist_append(&widgets, &widget->node);
    return 0;
}

lv_obj_t *zmk_widget_bongo_cat_obj(struct zmk_widget_bongo_cat *widget) { return widget->obj; }

ZMK_LISTENER(zmk_widget_bongo_cat, bongo_cat_listener)
ZMK_SUBSCRIPTION(zmk_widget_bongo_cat, zmk_keycode_state_changed);
