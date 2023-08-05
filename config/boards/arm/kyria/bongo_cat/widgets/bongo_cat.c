#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include "bongo_cat.h"

// #if IS_ENABLED(CONFIG_ZMK_DISPLAY_WORK_QUEUE_DEDICATED)
// #error "Bongo cat only works with the system work queue currently"
// #endif

#define ANIMATION_DEBOUNCE 10
#define IDLE_DELAY 150

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

enum anim_paw { anim_paw_left, anim_paw_right } last_paw;

enum anim_state {
    anim_state_none,
    anim_state_idle,
    anim_state_paws_up,
    anim_state_tapping,
} current_anim_state;

struct TimerData {
    void (*f)(void *);
    void *args;
};

const void **images;
uint8_t images_len;
lv_timer_t *current_timer;
lv_timer_t *listener_timer;
struct zmk_widget_bongo_cat *cat;

const void *idle_images[];
const void *paws_up_img;
const void *left_paw_img;
const void *right_paw_img;


void set_img_src(void *var, int val) {
    lv_obj_t *img = (lv_obj_t *)var;
    lv_img_set_src(img, images[val]);
}

void set_img(int img) { lv_img_set_src(cat->obj, img); }

void cancel_animation() {
    lv_anim_del(cat->obj, set_img_src);
    // lv_anim_del_all();
    // lv_timer_del(current_timer);
}

void timer_cb(lv_timer_t *timer) {
    struct TimerData *data = timer->user_data;
    data->f(data->args);
}

void timer_wrapper(void (*f)(void *), int delay, void *args) {
    // lv_timer_t *timer = lv_timer_create(*f, delay, args);
    current_timer = lv_timer_create(*f, delay, args);
    lv_timer_set_repeat_count(current_timer, 1);
}

void timer(void (*f)(void *), int delay, void *args) {
    struct TimerData data = {*f, args};
    timer_wrapper(timer_cb, delay, &data);
}

void play_idle_animation(void *null) {

    if (current_anim_state == anim_state_idle)
        return;

    // cancel_animation();
    set_img(&paws_up_img);

    current_anim_state = anim_state_idle;

    LOG_DBG("Idle animation");
    images = idle_images;
    lv_anim_init(&cat->anim);
    lv_anim_set_early_apply(&cat->anim, false);
    lv_anim_set_delay(&cat->anim, IDLE_DELAY);
    lv_anim_set_var(&cat->anim, cat->obj);
    lv_anim_set_time(&cat->anim, 800);
    lv_anim_set_values(&cat->anim, 0, 4);
    lv_anim_set_exec_cb(&cat->anim, set_img_src);
    lv_anim_set_repeat_count(&cat->anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&cat->anim, 100);
    lv_anim_start(&cat->anim);
}

void update_bongo_cat_wpm(int keyevent) {
    LOG_DBG("anim state %d", current_anim_state);
    // cancel_animation();
    // lv_timer_del(current_timer);
    if (!keyevent) {

        lv_timer_t *timer = lv_timer_create(play_idle_animation, 50, NULL);
        lv_timer_set_repeat_count(timer, 1);
        current_timer = timer;
        // timer_wrapper(play_idle_animation, 50, NULL);
        // play_idle_animation(0);
        // set_img(&paws_up_img);

    } else if (last_paw != anim_paw_left) {
        // cancel_animation();
        current_anim_state = anim_state_tapping;
        last_paw = anim_paw_left;
        if (current_anim_state == anim_state_idle) {
            set_img(&paws_up_img);
            timer(set_img, 300, &left_paw_img);
        } else {
            set_img(&left_paw_img);
        }

    } else {
        // cancel_animation();
        current_anim_state = anim_state_tapping;
        last_paw = anim_paw_right;
        if (current_anim_state == anim_state_idle) {
            set_img(&paws_up_img);
            timer(set_img, 300, &right_paw_img);
        } else {
            set_img(&right_paw_img);
        }
    }
}

void update_wrapper(lv_timer_t *timer) { update_bongo_cat_wpm(timer->user_data); }

int zmk_widget_bongo_cat_init(struct zmk_widget_bongo_cat *widget, lv_obj_t *parent) {

    LV_IMG_DECLARE(idle_img1);
    LV_IMG_DECLARE(idle_img2);
    LV_IMG_DECLARE(idle_img3);
    LV_IMG_DECLARE(idle_img4);
    LV_IMG_DECLARE(idle_img5);

    LV_IMG_DECLARE(paws_up_img);

    LV_IMG_DECLARE(right_paw_img);
    LV_IMG_DECLARE(left_paw_img);

    *idle_images[] = {
        &idle_img1, &idle_img2, &idle_img3, &idle_img4, &idle_img5,
    };


    widget->obj = lv_img_create(parent);
    cat = widget;
    listener_timer = lv_timer_create_basic();
    lv_timer_set_cb(listener_timer, update_wrapper);
    lv_timer_set_period(listener_timer, ANIMATION_DEBOUNCE);
    lv_timer_set_repeat_count(listener_timer, 1);

    play_idle_animation(0);
    sys_slist_append(&widgets, &widget->node);
    return 0;
}

lv_obj_t *zmk_widget_bongo_cat_obj(struct zmk_widget_bongo_cat *widget) { return widget->obj; }

int bongo_cat_listener(const zmk_event_t *eh) {
    // struct zmk_widget_bongo_cat *widget;

    const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
    // LOG_INF("------------- state %d - -------------", ev->state);
    // LOG_INF("------------- keycode %d ------------", ev->keycode);
    // LOG_INF("------------- usage %d --------------", ev->usage_page);
    // LOG_INF("------------- implicit %d -----------", ev->implicit_modifiers);
    // LOG_INF("------------- explicit %d -----------", ev->explicit_modifiers);
    // LOG_INF("------------- time %d ----------", ev->timestamp);
    // SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
    //     cat = widget;
    // }

    // if (listener_timer)
    //     lv_timer_del(listener_timer);

    // if (ev){
    // listener_timer->user_data = &ev->state;
    // }
    // lv_timer_reset(listener_timer);
    // lv_timer_ready(listener_timer);
    // listener_timer = lv_timer_create(update_wrapper, 10, &ev->state);
    // lv_timer_set_repeat_count(listener_timer, 1);
    update_bongo_cat_wpm(ev->state);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(zmk_widget_bongo_cat, bongo_cat_listener)
ZMK_SUBSCRIPTION(zmk_widget_bongo_cat, zmk_keycode_state_changed);
