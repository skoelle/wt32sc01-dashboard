#include "home_screen.h"
#include "ui/widgets/tile_button.h"
#include "icons/icons.h"
#include "api/weather_api.h"
#include "api/calendar_api.h"
#include <theme.h>
#include <text_utils.h>
#include <date_utils.h>
#include <lvgl.h>

namespace {

// --- navigation callback target (set by main.cpp) ---
ScreenId (*g_navigate)(ScreenId) = nullptr;

// Forward declarations (definitions below).
void updateWidgets();
void doFetch();

void on_calendar_tile(lv_event_t *) { if (g_navigate) g_navigate(ScreenId::CALENDAR_DETAIL); }
void on_mvg_tile(lv_event_t *) { if (g_navigate) g_navigate(ScreenId::MVG); }

// --- cached data + refresh timer (ported from old home_screen.cpp) ---
WeatherData lastWeather;
CalendarData lastCalendar;
bool weatherOk = false;
bool calendarOk = false;
unsigned long lastFetchMs = 0;
const unsigned long REFRESH_INTERVAL_MS = 10UL * 60UL * 1000UL;

// --- tile handles (value labels, for updating without rebuild) ---
TileButton weatherTile{};
TileButton calendarTile{};
TileButton mvgTile{};
bool widgetsBuilt = false;
lv_obj_t *rainWarningIcon = nullptr;

// Rebuild the weather canvas icon (deleted + recreated on each refresh
// because the symbol may change).
void rebuildWeatherIcon(const String &symbol, const String &description) {
    lv_obj_t *ic = Icons::createWeatherIcon(weatherTile.btn, symbol, description, 72);
    tile_button_set_icon_obj(weatherTile, ic);
    lv_obj_align(weatherTile.icon, LV_ALIGN_CENTER, 0, -14);
}

void updateRainWarning() {
    if (rainWarningIcon) {
        lv_obj_del(rainWarningIcon);
        rainWarningIcon = nullptr;
    }
    if (weatherOk && willRainSoon(lastWeather, 8)) {
        rainWarningIcon = Icons::createRainWarning(weatherTile.btn, 28);
        lv_obj_align(rainWarningIcon, LV_ALIGN_TOP_RIGHT, -4, 4);
    }
}

void doFetch() {
    WeatherData w = fetchWeather();
    weatherOk = w.valid;
    if (w.valid) lastWeather = w;
    CalendarData c = fetchCalendar();
    calendarOk = c.valid;
    if (c.valid) lastCalendar = c;
    lastFetchMs = millis();
}

// Manual retry: tapping the weather tile when it is in error state triggers
// a refresh instead of navigating away. Acts as the SPEC.md "retry per tap".
void on_weather_tile(lv_event_t *e) {
    if (!weatherOk) {
        doFetch();
        updateWidgets();
        return;
    }
    if (g_navigate) g_navigate(ScreenId::WEATHER_DETAIL);
}

// --- formatting helpers (ported from old home_screen.cpp rendering) ---

String weatherIconGlyph(const String &symbol) {
    // Kept for fallback; real icons are drawn as canvas via rebuildWeatherIcon.
    (void)symbol;
    return LV_SYMBOL_IMAGE;
}

String formatWeatherValue(const WeatherData &w) {
    if (!w.valid) return "Wetter n.a.";
    String s = String(w.current.temperature) + " C";
    s += "\n" + sanitizeGermanText(w.current.description);
    return s;
}

String formatCalendarPreview(const CalendarData &c) {
    if (!c.valid) return "Kalender n.a.";
    String s;
    int shown = 0;
    for (const auto &ev : c.events) {
        if (shown >= 2) break;
        if (shown > 0) s += "\n\n";
        s += DateUtils::formatShortDE(ev.startAt, ev.allDay);
        s += "\n" + sanitizeGermanText(ev.summary.substring(0, 22));
        ++shown;
    }
    if (s.length() == 0) s = "Keine Termine";
    return s;
}

void updateWidgets() {
    if (!widgetsBuilt) return;
    rebuildWeatherIcon(lastWeather.current.symbol, lastWeather.current.description);
    updateRainWarning();
    tile_button_set_value(weatherTile, formatWeatherValue(lastWeather).c_str());
    lv_obj_align(weatherTile.value, LV_ALIGN_CENTER, 0, 50);
    tile_button_set_value(calendarTile, formatCalendarPreview(lastCalendar).c_str());
    tile_button_set_icon(calendarTile, LV_SYMBOL_FILE);
    tile_button_set_value(mvgTile, "Abfahrten");
}

} // namespace

Screen homeScreen;

void homeScreen_setNavigator(Navigator nav) {
    g_navigate = nav;
}

void homeScreen_create(Screen &s) {
    // Layout (portrait 320x480):
    //  weather tile   x=12 y=12  w=296 h=170  (big, top)
    //  calendar tile  x=12 y=194 w=296 h=170  (big, middle, with 2-event preview)
    //  mvg tile       x=12 y=376 w=296 h=92   (small, bottom, no preview)
    lv_color_t tileBg = Theme::bgCard();

    weatherTile  = tile_button_create(s.root, 12, 12,  296, 170,
                                      tileBg, "Wetter",
                                      on_weather_tile, nullptr);
    lv_obj_set_width(weatherTile.value, 260);
    lv_obj_set_style_text_align(weatherTile.value, LV_TEXT_ALIGN_CENTER, 0);

    calendarTile = tile_button_create(s.root, 12, 194, 296, 170,
                                      tileBg, "Termine",
                                      on_calendar_tile, nullptr);

    mvgTile      = tile_button_create(s.root, 12, 376, 296, 92,
                                      tileBg, "MVG",
                                      on_mvg_tile, nullptr);
    widgetsBuilt = true;
}

void homeScreen_refresh(Screen &) {
    doFetch();
    updateWidgets();
}

void homeScreen_tick(Screen &) {
    if (lastFetchMs == 0 || millis() - lastFetchMs >= REFRESH_INTERVAL_MS) {
        doFetch();
        updateWidgets();
    }
}

Screen homeScreen_make() {
    Screen s;
    s.create_fn  = homeScreen_create;
    s.refresh_fn = homeScreen_refresh;
    s.tick_fn    = homeScreen_tick;
    return s;
}

