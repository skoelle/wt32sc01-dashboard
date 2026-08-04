// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#pragma once

// WT32-SC01 Plus pin assignments (ESP32-S3, ST7796 8-bit parallel, FT6336U touch).
// Verified against WT32-SC01 Plus schematic.

namespace BoardPins {

// --- ST7796 8-bit parallel RGB/8080 interface ---
constexpr int LCD_D0 = 9;
constexpr int LCD_D1 = 46;
constexpr int LCD_D2 = 3;
constexpr int LCD_D3 = 8;
constexpr int LCD_D4 = 18;
constexpr int LCD_D5 = 17;
constexpr int LCD_D6 = 16;
constexpr int LCD_D7 = 15;

constexpr int LCD_WR = 47;
constexpr int LCD_RD = -1;   // not wired
constexpr int LCD_DC = 0;    // RS/DC
constexpr int LCD_CS = -1;   // not wired
constexpr int LCD_RST = 4;
constexpr int LCD_BACKLIGHT = 45;

// --- FT6336U capacitive touch (I2C) ---
constexpr int TOUCH_SDA = 6;
constexpr int TOUCH_SCL = 5;
constexpr int TOUCH_INT = 7;

} // namespace BoardPins
