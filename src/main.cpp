#include <Arduino.h>
#include <WiFi.h>
#include <lvgl.h>
#include <secrets.h>
#include <theme.h>
#include "display/display_setup.h"
#include "ui/screen_base.h"
#include "ui/home_screen.h"
#include "ui/weather_detail_screen.h"
#include "ui/calendar_detail_screen.h"
#include "ui/mvg_screen.h"

namespace {

unsigned long lastTickMs = 0;
unsigned long lastInteractionMs = 0;
const unsigned long IDLE_TIMEOUT_MS = 5UL * 60UL * 1000UL;

Screen screens[4];
ScreenId current = ScreenId::HOME;

ScreenId navigate(ScreenId target) {
    lastInteractionMs = millis();
    current = target;
    screens[(int)target].show();
    return target;
}

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

} // namespace

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("WT32-SC01 Plus booting...");
    display_init();
    connectWifi();

    // Build all four screens and wire navigation callbacks.
    screens[(int)ScreenId::HOME]            = homeScreen_make();
    screens[(int)ScreenId::WEATHER_DETAIL]  = weatherDetailScreen_make();
    screens[(int)ScreenId::CALENDAR_DETAIL] = calendarDetailScreen_make();
    screens[(int)ScreenId::MVG]             = mvgScreen_make();
    homeScreen_setNavigator(navigate);
    weatherDetailScreen_setNavigator(navigate);
    calendarDetailScreen_setNavigator(navigate);
    mvgScreen_setNavigator(navigate);

    lastInteractionMs = millis();
    navigate(ScreenId::HOME);
    Serial.println("Setup done.");
}

void loop() {
    unsigned long now = millis();
    if (now - lastTickMs >= 5) {
        lastTickMs = now;
        display_loop();
        screens[(int)current].tick();

        // Inactivity: return to home after 5 minutes without input.
        // Use millis() directly — lastInteractionMs may have been updated
        // during display_loop() inside this same iteration; using the stale
        // `now` from loop start would underflow and trigger immediately.
        if (current != ScreenId::HOME &&
            millis() - lastInteractionMs >= IDLE_TIMEOUT_MS) {
            navigate(ScreenId::HOME);
        }
    }
}
