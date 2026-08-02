#include "back_button.h"
#include <theme.h>

lv_obj_t *back_button_create(lv_obj_t *parent, lv_event_cb_t on_click,
                             void *user_data) {
    lv_obj_t *btn = lv_button_create(parent);
    // Fixed at bottom-left, large enough to be a reliable touch target.
    lv_obj_set_pos(btn, 12, 480 - 12 - 60);
    lv_obj_set_size(btn, 100, 60);
    lv_obj_set_style_bg_color(btn, Theme::bgCard(), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_radius(btn, 14, 0);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_LEFT " Zurueck");
    lv_obj_set_style_text_color(label, Theme::text(), 0);
    lv_obj_center(label);

    if (on_click) {
        lv_obj_add_event_cb(btn, on_click, LV_EVENT_CLICKED, user_data);
    }
    return btn;
}
