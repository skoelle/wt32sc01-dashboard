#include "weather_detail_screen.h"
#include "ui/widgets/back_button.h"
#include "api/weather_api.h"
#include <theme.h>
#include <text_utils.h>
#include <lvgl.h>

namespace {

ScreenId (*g_navigate)(ScreenId) = nullptr;
void on_back(lv_event_t *) { if (g_navigate) g_navigate(ScreenId::HOME); }

WeatherData lastWeather;
bool ok = false;
lv_obj_t *list = nullptr;

void buildList() {
    if (!list) return;
    lv_obj_clean(list);

    if (!ok) {
        lv_obj_t *lbl = lv_label_create(list);
        lv_label_set_text(lbl, LV_SYMBOL_WARNING " Keine Verbindung");
        lv_obj_set_style_text_color(lbl, Theme::accentError(), 0);
        return;
    }

    // Header row with current conditions.
    String header = String(lastWeather.current.temperature) + " C  " +
                    sanitizeGermanText(lastWeather.current.description);
    lv_obj_t *h = lv_label_create(list);
    lv_label_set_text(h, header.c_str());
    lv_obj_set_style_text_font(h, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(h, Theme::text(), 0);

    for (const auto &fe : lastWeather.forecast) {
        lv_obj_t *row = lv_obj_create(list);
        lv_obj_set_size(row, 280, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_all(row, 6, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        // time hh:mm
        int tIdx = fe.time.indexOf('T');
        String hhmm = tIdx > 0 ? fe.time.substring(tIdx + 1, tIdx + 6) : fe.time;

        String left = hhmm + "  " + String(fe.temperature) + "C";
        lv_obj_t *l = lv_label_create(row);
        lv_label_set_text(l, left.c_str());
        lv_obj_set_style_text_color(l, Theme::text(), 0);
        lv_obj_align(l, LV_ALIGN_TOP_LEFT, 0, 0);

        String desc = sanitizeGermanText(fe.description.substring(0, 16));
        if (fe.precipitationType == "rain" && fe.precipitationProbability > 0.0f) {
            char buf[8];
            snprintf(buf, sizeof(buf), " %.0f%%", fe.precipitationProbability * 100);
            desc += buf;
        }
        lv_obj_t *d = lv_label_create(row);
        lv_label_set_text(d, desc.c_str());
        lv_obj_set_style_text_color(d, Theme::textDim(), 0);
        lv_obj_align(d, LV_ALIGN_TOP_LEFT, 0, 18);
    }
}

} // namespace

void weatherDetailScreen_setNavigator(ScreenId (*nav)(ScreenId)) { g_navigate = nav; }

void weatherDetailScreen_create(Screen &s) {
    list = lv_list_create(s.root);
    lv_obj_set_pos(list, 0, 0);
    lv_obj_set_size(list, 320, 480 - 80);
    lv_obj_set_style_bg_color(list, Theme::bg(), 0);
    lv_obj_set_style_border_width(list, 0, 0);
    back_button_create(s.root, on_back, nullptr);
}

void weatherDetailScreen_refresh(Screen &) {
    WeatherData w = fetchWeather();
    ok = w.valid;
    if (w.valid) lastWeather = w;
    buildList();
}

void weatherDetailScreen_tick(Screen &) {}

Screen weatherDetailScreen_make() {
    Screen s;
    s.create_fn  = weatherDetailScreen_create;
    s.refresh_fn = weatherDetailScreen_refresh;
    s.tick_fn    = weatherDetailScreen_tick;
    return s;
}
