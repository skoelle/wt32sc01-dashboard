// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#pragma once
#include <lvgl.h>

// Reusable "Zurueck" button fixed at bottom-left of a detail screen.
// Fires the given callback on tap (typically: navigate back to home).
lv_obj_t *back_button_create(lv_obj_t *parent, lv_event_cb_t on_click,
                             void *user_data);
