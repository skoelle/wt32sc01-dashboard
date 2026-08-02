#pragma once
#include <lvgl.h>
#include <theme.h>

// Reusable large touch tile for the home screen. Icon + title + optional
// value/preview lines, accent background per category. Fires a callback on tap.
struct TileButton {
    lv_obj_t *btn = nullptr;
    lv_obj_t *icon = nullptr;
    lv_obj_t *title = nullptr;
    lv_obj_t *value = nullptr;
};

// Create a tile button inside `parent` at (x,y) with given size and accent.
// title: heading text (e.g. "Wetter"); on_click + user_data wired to LV_EVENT_CLICKED.
TileButton tile_button_create(lv_obj_t *parent, lv_coord_t x, lv_coord_t y,
                              lv_coord_t w, lv_coord_t h, lv_color_t accent,
                              const char *title, lv_event_cb_t on_click,
                              void *user_data);

// Set the big value/preview text on the tile (may be multi-line).
void tile_button_set_value(TileButton &t, const char *text);

// Set the icon text (single glyph/symbol). Icons are rendered as text labels
// using LVGL's built-in font until bitmap icons land in Phase 6.
void tile_button_set_icon(TileButton &t, const char *text);

// Replace the text icon with an already-created canvas object (the caller
// creates the canvas as a child of t.btn). Useful for bitmap/procedural icons.
void tile_button_set_icon_obj(TileButton &t, lv_obj_t *icon_obj);
