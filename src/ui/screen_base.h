#pragma once
#include <lvgl.h>
#include <theme.h>

enum class ScreenId {
    HOME,
    WEATHER_DETAIL,
    CALENDAR_DETAIL,
    MVG
};

// Common lifecycle for all screens. Each screen owns one lv_obj_t screen
// object; create() builds the UI once, show()/hide() toggle visibility,
// refresh() re-fetches data and updates the widgets, tick() is called
// every loop iteration for periodic refresh checks.
struct Screen {
    lv_obj_t *root = nullptr;
    lv_obj_t *loadingLabel = nullptr;
    bool created = false;

    void (*create_fn)(Screen &) = nullptr;
    void (*refresh_fn)(Screen &) = nullptr;
    void (*tick_fn)(Screen &) = nullptr;

    void create() {
        if (created || !create_fn) return;
        root = lv_obj_create(nullptr);
        lv_obj_set_size(root, 320, 480);
        lv_obj_set_style_bg_color(root, lv_color_hex(0x000000), 0);
        lv_obj_set_style_border_width(root, 0, 0);
        lv_obj_set_style_pad_all(root, 0, 0);
        lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
        create_fn(*this);

        loadingLabel = lv_label_create(root);
        lv_label_set_text(loadingLabel, LV_SYMBOL_REFRESH " Lädt...");
        lv_obj_set_style_text_color(loadingLabel, Theme::text(), 0);
        lv_obj_set_style_bg_color(loadingLabel, Theme::bg(), 0);
        lv_obj_set_style_bg_opa(loadingLabel, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(loadingLabel, 8, 0);
        lv_obj_set_style_pad_all(loadingLabel, 8, 0);
        lv_obj_set_align(loadingLabel, LV_ALIGN_BOTTOM_RIGHT);
        lv_obj_align(loadingLabel, LV_ALIGN_BOTTOM_RIGHT, -12, -12);
        lv_obj_add_flag(loadingLabel, LV_OBJ_FLAG_HIDDEN);

        created = true;
    }

    void showLoading(bool visible) {
        if (!loadingLabel) return;
        if (visible) {
            lv_obj_clear_flag(loadingLabel, LV_OBJ_FLAG_HIDDEN);
            lv_obj_move_foreground(loadingLabel);
        } else {
            lv_obj_add_flag(loadingLabel, LV_OBJ_FLAG_HIDDEN);
        }
    }

    void show() {
        if (!created) create();
        lv_screen_load(root);
        lv_indev_reset(NULL, root);
        showLoading(true);
        lv_timer_handler();
        refresh();
        showLoading(false);
        lv_timer_handler();
    }

    void hide() {}

    void refresh() {
        if (created && refresh_fn) refresh_fn(*this);
    }

    void tick() {
        if (created && tick_fn) tick_fn(*this);
    }
};
