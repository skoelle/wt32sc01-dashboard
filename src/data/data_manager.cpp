#include "data_manager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace {

constexpr unsigned long REFRESH_INTERVAL_MS = 10UL * 60UL * 1000UL;
constexpr uint32_t TASK_STACK_WORDS = 16384; // 64 KB — JSON parsing needs headroom

// --- Shared state between background task and main loop ---
// The task is the only writer of s_weather/s_calendar; the main loop is the
// only reader. Access to the data structs is serialized via s_mux (a
// portMUX_TYPE critical section which, on ESP32, suspends task switches on
// the current core). Simple flag/counter reads (bool/uint32_t) are atomic
// on 32-bit Xtensa and only need `volatile`.
static WeatherData s_weather;
static CalendarData s_calendar;
static volatile bool s_weatherOk = false;
static volatile bool s_calendarOk = false;
static volatile bool s_loading = false;
static volatile uint32_t s_dataVersion = 0;
static volatile unsigned long s_lastFetchMs = 0;
static volatile bool s_refreshRequested = false;
static portMUX_TYPE s_mux = portMUX_INITIALIZER_UNLOCKED;

static void dataFetchTask(void *param) {
    (void)param;
    while (true) {
        bool needRefresh = s_refreshRequested ||
                           s_lastFetchMs == 0 ||
                           (millis() - s_lastFetchMs) >= REFRESH_INTERVAL_MS;

        if (needRefresh) {
            s_refreshRequested = false;
            s_loading = true;

            WeatherData w = fetchWeather();
            CalendarData c = fetchCalendar();

            portENTER_CRITICAL(&s_mux);
            if (w.valid) {
                s_weather = std::move(w);
                s_weatherOk = true;
            } else {
                s_weatherOk = false;
            }
            if (c.valid) {
                s_calendar = std::move(c);
                s_calendarOk = true;
            } else {
                s_calendarOk = false;
            }
            s_dataVersion++;
            s_lastFetchMs = millis();
            s_loading = false;
            portEXIT_CRITICAL(&s_mux);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

} // namespace

void dataManager_begin() {
    s_lastFetchMs = 0; // ensure first iteration fetches immediately
    xTaskCreatePinnedToCore(dataFetchTask, "dataFetch", TASK_STACK_WORDS,
                            nullptr, 1, nullptr, 1);
}

void dataManager_tick() {}

void dataManager_triggerRefresh() {
    s_refreshRequested = true;
}

void dataManager_getWeather(WeatherData &out) {
    portENTER_CRITICAL(&s_mux);
    out = s_weather;
    portEXIT_CRITICAL(&s_mux);
}

void dataManager_getCalendar(CalendarData &out) {
    portENTER_CRITICAL(&s_mux);
    out = s_calendar;
    portEXIT_CRITICAL(&s_mux);
}

bool dataManager_isWeatherOk()    { return s_weatherOk; }
bool dataManager_isCalendarOk()   { return s_calendarOk; }
bool dataManager_isLoading()      { return s_loading; }
uint32_t dataManager_dataVersion() { return s_dataVersion; }
unsigned long dataManager_lastFetchMs() { return s_lastFetchMs; }
