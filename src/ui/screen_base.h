// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#pragma once
#include <lvgl.h>

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
    bool created = false;

    // Build the screen object tree (hidden). Called once at startup.
    void (*create_fn)(Screen &) = nullptr;
    // Re-fetch data and update widgets.
    void (*refresh_fn)(Screen &) = nullptr;
    // Per-loop tick: check refresh timers / inactivity.
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
        created = true;
    }

    void show() {
        if (!created) create();
        lv_screen_load(root);
        lv_indev_reset(NULL, root);
        refresh();
    }

    void hide() {}

    void refresh() {
        if (created && refresh_fn) refresh_fn(*this);
    }

    void tick() {
        if (created && tick_fn) tick_fn(*this);
    }
};
