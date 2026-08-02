#pragma once

// WT32-SC01 Plus pin assignments.
// Reference values per WT32-SC01 Plus documentation (ESP32-S3-WROVER,
// ST7796 8-bit parallel + FT6336U capacitive touch). Verify against the
// concrete board revision before production use (see SPEC.md section 11).
//
// These constants are consumed by src/display/display_setup.cpp only.

namespace BoardPins {

// --- ST7796 8-bit parallel RGB/8080 interface ---
constexpr int LCD_D0 = 8;
constexpr int LCD_D1 = 3;
constexpr int LCD_D2 = 46;
constexpr int LCD_D3 = 9;
constexpr int LCD_D4 = 1;
constexpr int LCD_D5 = 5;
constexpr int LCD_D6 = 6;
constexpr int LCD_D7 = 7;

constexpr int LCD_WR = 39;
constexpr int LCD_RD = 38;
constexpr int LCD_DC = 40;
constexpr int LCD_CS = 41;
constexpr int LCD_RST = 42;
constexpr int LCD_BACKLIGHT = 2;

// --- FT6336U capacitive touch (I2C) ---
constexpr int TOUCH_SDA = 14;
constexpr int TOUCH_SCL = 12;
constexpr int TOUCH_INT = 13;

} // namespace BoardPins
