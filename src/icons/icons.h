#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include <theme.h>

// Colored procedural icons drawn onto LVGL canvases (no external bitmap
// assets). Replaces the old M5GFX-based icons.h. Each builder returns an
// lv_obj_t* canvas of the given size, drawn once at create time.
namespace Icons {

// Weather icon selected from the API `symbol` field (m=night, condition
// char = symbol[1]: o=clear, b=cloudy, r=rain, t=thunder).
lv_obj_t *createWeatherIcon(lv_obj_t *parent, const String &symbol,
                            const String &description, int size);

// Rain warning triangle (for the weather tile badge).
lv_obj_t *createRainWarning(lv_obj_t *parent, int size);

// Retry / error symbol (circle + exclamation).
lv_obj_t *createRetryIcon(lv_obj_t *parent, int size);

// Back arrow (used if a non-symbol back button variant is desired).
lv_obj_t *createBackArrow(lv_obj_t *parent, int size);

} // namespace Icons
