#include <Arduino.h>
#include <WiFi.h>
#include <lvgl.h>
#include <secrets.h>
#include <theme.h>
#include "display/display_setup.h"
#include "api/weather_api.h"
#include "api/calendar_api.h"
#include "api/departures_api.h"

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

// Phase 2 verification: fetch all three APIs once at boot and dump parsed
// fields to Serial so JSON parsing can be verified independently of the UI.
void verifyApisViaSerial() {
    Serial.println("\n=== Wetter ===");
    WeatherData w = fetchWeather();
    Serial.printf("valid=%d temp=%d symbol=%s desc=%s forecast=%u\n",
                  w.valid, w.current.temperature,
                  w.current.symbol.c_str(), w.current.description.c_str(),
                  w.forecast.size());
    for (size_t i = 0; i < w.forecast.size() && i < 3; ++i) {
        const auto &f = w.forecast[i];
        Serial.printf("  [%u] %s %dC %s precip=%.2f/%s\n", i, f.time.c_str(),
                      f.temperature, f.description.c_str(),
                      f.precipitationProbability, f.precipitationType.c_str());
    }

    Serial.println("\n=== Kalender ===");
    CalendarData c = fetchCalendar();
    Serial.printf("valid=%d events=%u\n", c.valid, c.events.size());
    for (size_t i = 0; i < c.events.size() && i < 3; ++i) {
        const auto &e = c.events[i];
        Serial.printf("  [%u] %s | %s all_day=%d\n", i, e.summary.c_str(),
                      e.startAt.c_str(), e.allDay);
    }

    Serial.println("\n=== MVG ===");
    DeparturesData d = fetchDepartures();
    Serial.printf("valid=%d departures=%u\n", d.valid, d.departures.size());
    for (size_t i = 0; i < d.departures.size() && i < 3; ++i) {
        const auto &dep = d.departures[i];
        Serial.printf("  [%u] %s %s -> %s %s delay=%d cancel=%d\n", i,
                      dep.type.c_str(), dep.line.c_str(),
                      dep.destination.c_str(), dep.timeStr.c_str(),
                      dep.delayMin, dep.cancelled);
    }
    Serial.println("=== API verification done ===\n");
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
    verifyApisViaSerial();
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
