#pragma once
#include <lvgl.h>

// Dark-mode theme constants and tile accent colors for the WT32-SC01 Plus
// dashboard. Replaces the former M5Stack-specific theme.h; the same RGB565
// values are kept as 16-bit literals so they can be reused both for LVGL
// (via lv_color_hex) and for direct LovyanGFX drawing if needed.
namespace Theme {

// Logical portrait resolution after rotation (320 wide x 480 tall).
constexpr int SCREEN_W = 320;
constexpr int SCREEN_H = 480;

// Base palette (RGB565 kept for reference; LVGL uses the hex forms below).
//   BG        0x0000  pure black
//   BG_CARD   0x1082  very dark grey (tile / separator)
//   TEXT      0xFFFF  white
//   TEXT_DIM  0x8410  mid grey
inline lv_color_t bg()        { return lv_color_hex(0x000000); }
inline lv_color_t bgCard()    { return lv_color_hex(0x1C1C1E); }
inline lv_color_t text()      { return lv_color_hex(0xFFFFFF); }
inline lv_color_t textDim()   { return lv_color_hex(0x9A9A9E); }

// Tile accent backgrounds (muted, iOS-dark-mode style).
inline lv_color_t accentWeather()  { return lv_color_hex(0x1B3A5C); } // muted blue
inline lv_color_t accentCalendar() { return lv_color_hex(0x3A2A5C); } // muted purple
inline lv_color_t accentMvg()      { return lv_color_hex(0x2C5C3A); } // muted green

// Symbol / status colors.
inline lv_color_t accentSun()      { return lv_color_hex(0xFBC02D); }
inline lv_color_t accentRain()     { return lv_color_hex(0x4FC3F7); }
inline lv_color_t accentUBahn()    { return lv_color_hex(0xE62D2D); } // MVV U-Bahn red
inline lv_color_t accentSBahn()    { return lv_color_hex(0x1A7A3A); } // MVV S-Bahn green
inline lv_color_t accentWarn()     { return lv_color_hex(0xFFB300); }
inline lv_color_t accentError()    { return lv_color_hex(0xF44336); }
inline lv_color_t accentStorm()    { return lv_color_hex(0xFFD54F); }

} // namespace Theme
