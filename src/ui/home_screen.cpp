#include "home_screen.h"
#include "ui/widgets/tile_button.h"
#include "icons/icons.h"
#include "data/data_manager.h"
#include <theme.h>
#include <text_utils.h>
#include <date_utils.h>
#include <lvgl.h>

namespace {

// --- navigation callback target (set by main.cpp) ---
ScreenId (*g_navigate)(ScreenId) = nullptr;

void on_calendar_tile(lv_event_t *) { if (g_navigate) g_navigate(ScreenId::CALENDAR_DETAIL); }
void on_mvg_tile(lv_event_t *) { if (g_navigate) g_navigate(ScreenId::MVG); }

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

void updateRainWarning(const WeatherData &w, bool ok) {
    if (rainWarningIcon) {
        lv_obj_del(rainWarningIcon);
        rainWarningIcon = nullptr;
    }
    if (ok && willRainSoon(w, 8)) {
        rainWarningIcon = Icons::createRainWarning(weatherTile.btn, 28);
        lv_obj_align(rainWarningIcon, LV_ALIGN_TOP_RIGHT, -4, 4);
    }
}

// Manual retry: tapping the weather tile when it is in error state triggers
// a background refresh instead of navigating away.
void on_weather_tile(lv_event_t *) {
    if (!dataManager_isWeatherOk()) {
        dataManager_triggerRefresh();
        return;
    }
    if (g_navigate) g_navigate(ScreenId::WEATHER_DETAIL);
}

// --- formatting helpers ---

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
        if (shown > 0) s += "\n";
        s += sanitizeGermanText(ev.summary.substring(0, 22));
        s += "\n" + DateUtils::formatShortDE(ev.startAt, ev.allDay);
        ++shown;
    }
    if (s.length() == 0) s = "Keine Termine";
    return s;
}

void updateWidgets() {
    if (!widgetsBuilt) return;

    WeatherData w;
    CalendarData c;
    dataManager_getWeather(w);
    dataManager_getCalendar(c);
    bool weatherOk = dataManager_isWeatherOk();
    bool calendarOk = dataManager_isCalendarOk();
    bool loading = dataManager_isLoading();

    // Weather tile
    if (weatherOk) {
        rebuildWeatherIcon(w.current.symbol, w.current.description);
        updateRainWarning(w, true);
        tile_button_set_value(weatherTile, formatWeatherValue(w).c_str());
    } else if (loading) {
        tile_button_set_value(weatherTile, "Lade...");
    } else {
        tile_button_set_value(weatherTile, "Wetter n.a.");
    }
    lv_obj_align(weatherTile.value, LV_ALIGN_CENTER, 0, 50);

    // Calendar tile
    if (calendarOk) {
        tile_button_set_value(calendarTile, formatCalendarPreview(c).c_str());
    } else if (loading) {
        tile_button_set_value(calendarTile, "Lade...");
    } else {
        tile_button_set_value(calendarTile, "Kalender n.a.");
    }
    tile_button_set_icon(calendarTile, LV_SYMBOL_FILE);

    // MVG tile
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
    updateWidgets();
}

void homeScreen_tick(Screen &) {
    static uint32_t lastVersion = 0;
    uint32_t v = dataManager_dataVersion();
    if (v != lastVersion) {
        lastVersion = v;
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
