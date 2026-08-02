#include "tile_button.h"

TileButton tile_button_create(lv_obj_t *parent, lv_coord_t x, lv_coord_t y,
                              lv_coord_t w, lv_coord_t h, lv_color_t accent,
                              const char *title, lv_event_cb_t on_click,
                              void *user_data) {
    TileButton t;
    t.btn = lv_button_create(parent);
    lv_obj_set_pos(t.btn, x, y);
    lv_obj_set_size(t.btn, w, h);
    lv_obj_set_style_bg_color(t.btn, accent, 0);
    lv_obj_set_style_bg_opa(t.btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(t.btn, 0, 0);
    lv_obj_set_style_radius(t.btn, 20, 0);
    lv_obj_set_style_pad_all(t.btn, 16, 0);

    // Title (top-left).
    t.title = lv_label_create(t.btn);
    lv_label_set_text(t.title, title);
    lv_obj_set_style_text_color(t.title, Theme::text(), 0);
    lv_obj_set_style_text_font(t.title, &lv_font_montserrat_16, 0);
    lv_obj_align(t.title, LV_ALIGN_TOP_LEFT, 0, 0);

    // Icon (top-right).
    t.icon = lv_label_create(t.btn);
    lv_label_set_text(t.icon, "");
    lv_obj_set_style_text_color(t.icon, Theme::text(), 0);
    lv_obj_set_style_text_font(t.icon, &lv_font_montserrat_24, 0);
    lv_obj_align(t.icon, LV_ALIGN_TOP_RIGHT, 0, 0);

    // Value/preview (below title).
    t.value = lv_label_create(t.btn);
    lv_label_set_text(t.value, "");
    lv_obj_set_style_text_color(t.value, Theme::text(), 0);
    lv_obj_set_style_text_font(t.value, &lv_font_montserrat_20, 0);
    lv_obj_align(t.value, LV_ALIGN_LEFT_MID, 0, 10);

    if (on_click) {
        lv_obj_add_event_cb(t.btn, on_click, LV_EVENT_CLICKED, user_data);
    }
    return t;
}

void tile_button_set_value(TileButton &t, const char *text) {
    if (t.value) lv_label_set_text(t.value, text);
}

void tile_button_set_icon(TileButton &t, const char *text) {
    if (t.icon) lv_label_set_text(t.icon, text);
}
