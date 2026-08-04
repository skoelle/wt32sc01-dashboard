// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#include "display_setup.h"
#include <LovyanGFX.hpp>
#include <driver/i2c.h>
#include <board_pins.h>
#include <theme.h>

// Defined at file scope (not anonymous namespace) so it matches the
// `extern` declaration in display_setup.h without ambiguity.
lv_display_t *lv_display_gfx = nullptr;

namespace {

// --- LovyanGFX device for WT32-SC01 Plus ---
// ST7796 8-bit parallel panel + FT6336U (FT5x06 family) capacitive touch.
class LGfx : public lgfx::LGFX_Device {
    lgfx::Panel_ST7796  _panel_instance;
    lgfx::Bus_Parallel8 _bus_instance;
    lgfx::Light_PWM     _light_instance;
    lgfx::Touch_FT5x06  _touch_instance;

public:
    LGfx(void) {
        // 8-bit parallel i80/8080 bus (ESP32-S3 variant).
        {
            auto cfg = _bus_instance.config();
            cfg.freq_write = 20000000;
            cfg.freq_read  =  8000000;
            cfg.pin_wr = BoardPins::LCD_WR;
            cfg.pin_rd = BoardPins::LCD_RD;
            cfg.pin_rs = BoardPins::LCD_DC; // DC = RS
            cfg.pin_d0 = BoardPins::LCD_D0;
            cfg.pin_d1 = BoardPins::LCD_D1;
            cfg.pin_d2 = BoardPins::LCD_D2;
            cfg.pin_d3 = BoardPins::LCD_D3;
            cfg.pin_d4 = BoardPins::LCD_D4;
            cfg.pin_d5 = BoardPins::LCD_D5;
            cfg.pin_d6 = BoardPins::LCD_D6;
            cfg.pin_d7 = BoardPins::LCD_D7;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        // ST7796 panel geometry / control pins.
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs       = BoardPins::LCD_CS;
            cfg.pin_rst      = BoardPins::LCD_RST;
            cfg.pin_busy     = -1;
            cfg.panel_width  = 320;
            cfg.panel_height = 480;
            cfg.offset_x     = 0;
            cfg.offset_y     = 0;
            cfg.offset_rotation = 0;
            cfg.readable     = true;
            cfg.invert       = true;
            cfg.rgb_order    = false;
            cfg.dlen_16bit   = false;
            cfg.bus_shared   = true;
            cfg.memory_width  = 320;
            cfg.memory_height = 480;
            _panel_instance.config(cfg);
        }
        // Backlight via PWM.
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = BoardPins::LCD_BACKLIGHT;
            cfg.invert = false;
            cfg.freq   = 44100;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }
        // FT6336U touch (FT5x06 family) on I2C, addr 0x38 (default).
        {
            auto cfg = _touch_instance.config();
            cfg.x_min = 0;
            cfg.x_max = 319;
            cfg.y_min = 0;
            cfg.y_max = 479;
            cfg.pin_int = BoardPins::TOUCH_INT;
            cfg.bus_shared = true;
            cfg.offset_rotation = 0;
            cfg.i2c_port = I2C_NUM_0;
            cfg.pin_sda  = BoardPins::TOUCH_SDA;
            cfg.pin_scl  = BoardPins::TOUCH_SCL;
            cfg.i2c_addr = 0x38;
            cfg.freq     = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance);
    }
};

LGfx lcd;

// LVGL draw buffers in PSRAM (partial rendering, two buffers).
constexpr uint32_t LVGL_BUF_PIXELS = 320 * 80;
lv_color_t *lvgl_buf1 = nullptr;
lv_color_t *lvgl_buf2 = nullptr;

// --- LVGL display flush callback -> LovyanGFX ---
void lv_display_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    lcd.startWrite();
    lcd.setAddrWindow(area->x1, area->y1, w, h);
    lcd.writePixels((lgfx::rgb565_t *)px_map, w * h);
    lcd.endWrite();
    lv_display_flush_ready(disp);
}

// --- LVGL touch read callback -> LovyanGFX touch ---
void lv_touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
    lgfx::touch_point_t tp;
    if (lcd.getTouch(&tp)) {
        data->point.x = tp.x;
        data->point.y = tp.y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

} // namespace

void display_init() {
    lcd.init();
    lcd.setRotation(0); // Portrait 320x480 on WT32-SC01 Plus
    lcd.fillScreen(0x0000); // black (RGB565)
    lcd.setBrightness(180);

    // Allocate LVGL draw buffers in PSRAM.
    lvgl_buf1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LVGL_BUF_PIXELS, MALLOC_CAP_SPIRAM);
    lvgl_buf2 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LVGL_BUF_PIXELS, MALLOC_CAP_SPIRAM);

    lv_init();
    lv_display_gfx = lv_display_create(Theme::SCREEN_W, Theme::SCREEN_H);
    lv_display_set_color_format(lv_display_gfx, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(lv_display_gfx, lvgl_buf1, lvgl_buf2,
                           sizeof(lv_color_t) * LVGL_BUF_PIXELS,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(lv_display_gfx, lv_display_flush_cb);

    lv_theme_t *th = lv_theme_default_init(
        lv_display_gfx,
        Theme::bg(),
        Theme::text(),
        true,
        LV_FONT_DEFAULT);
    lv_display_set_theme(lv_display_gfx, th);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, lv_touch_read_cb);
    lv_indev_set_display(indev, lv_display_gfx);
}

void display_loop() {
    lv_tick_inc(5);
    lv_timer_handler();
}
