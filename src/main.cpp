#include <Arduino.h>
#include <WiFi.h>
#include <lvgl.h>
#include <secrets.h>
#include <theme.h>
#include "display/display_setup.h"

namespace {

unsigned long lastTickMs = 0;

void connectWifi() {
    Serial.println("Verbinde mit WLAN...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
        delay(300);
        Serial.print(".");
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("WLAN verbunden, IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WLAN fehlgeschlagen");
    }
}

void btn_event_cb(lv_event_t *e) {
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
    static int n = 0;
    lv_label_set_text_fmt(label, "Taps: %d", ++n);
}

void buildTestScreen() {
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, Theme::bg(), 0);

    lv_obj_t *btn = lv_button_create(scr);
    lv_obj_set_size(btn, 200, 80);
    lv_obj_center(btn);
    lv_obj_set_style_bg_color(btn, Theme::accentWeather(), 0);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Taps: 0");
    lv_obj_center(label);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, label);
}

} // namespace

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("WT32-SC01 Plus booting...");
    display_init();
    connectWifi();
    buildTestScreen();
    Serial.println("Setup done.");
}

void loop() {
    unsigned long now = millis();
    if (now - lastTickMs >= 5) {
        lastTickMs = now;
        display_loop();
    }
}
