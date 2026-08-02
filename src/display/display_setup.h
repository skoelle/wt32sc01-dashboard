#pragma once
#include <lvgl.h>

// Initializes the WT32-SC01 Plus display (LovyanGFX, ST7796 8-bit parallel,
// portrait 320x480) and the FT6336U capacitive touch, wires both into LVGL.
// Must be called once from setup() before any UI code.
void display_init();

// LVGL display driver instance (exposed so main loop can drive it).
extern lv_display_t *lv_display_gfx;

// Drive LVGL timers + touch readback. Call once per loop() iteration.
void display_loop();
