#include "weather_detail_screen.h"
#include "ui/widgets/back_button.h"
#include "api/weather_api.h"
#include "icons/icons.h"
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

    // Header row with column titles.
    lv_obj_t *header = lv_label_create(list);
    lv_label_set_text(header, "Icon  Uhrzeit  Grad  Text  Regen %");
    lv_obj_set_style_text_font(header, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(header, Theme::text(), 0);

    // Separator line below header.
    lv_obj_t *header_line = lv_line_create(list);
    static lv_point_precise_t header_points[] = {{0, 0}, {280, 0}};
    lv_line_set_points(header_line, header_points, 2);
    lv_obj_set_style_line_color(header_line, Theme::textDim(), 0);
    lv_obj_set_style_line_width(header_line, 1, 0);

    for (const auto &fe : lastWeather.forecast) {
        lv_obj_t *row = lv_obj_create(list);
        lv_obj_set_size(row, 280, 30);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_all(row, 4, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        // time hh:mm
        int tIdx = fe.time.indexOf('T');
        String hhmm = tIdx > 0 ? fe.time.substring(tIdx + 1, tIdx + 6) : fe.time;

        // icon - use weather symbol
        lv_obj_t *icon = Icons::createWeatherIcon(row, fe.symbol, fe.description, 24);
        lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 10, 0);

        // time column
        lv_obj_t *time_label = lv_label_create(row);
        lv_label_set_text(time_label, hhmm.c_str());
        lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
        lv_obj_align(time_label, LV_ALIGN_TOP_LEFT, 44, 0);

        // degree column
        lv_obj_t *temp_label = lv_label_create(row);
        lv_label_set_text(temp_label, (String(fe.temperature) + "C").c_str());
        lv_obj_set_style_text_color(temp_label, lv_color_hex(0xFFFFFF), 0);
        lv_obj_align(temp_label, LV_ALIGN_TOP_LEFT, 160, 0);

        // text column
        String desc = sanitizeGermanText(fe.description.substring(0, 20));
        if (fe.precipitationType == "rain" && fe.precipitationProbability > 0.0f) {
            char buf[8];
            snprintf(buf, sizeof(buf), " %.0f%%", fe.precipitationProbability * 100);
            desc += buf;
        }
        lv_obj_t *desc_label = lv_label_create(row);
        lv_label_set_text(desc_label, desc.c_str());
        lv_obj_set_style_text_color(desc_label, fe.precipitationType == "rain" ? Theme::accentRain() : Theme::textDim(), 0);
        lv_obj_align(desc_label, LV_ALIGN_TOP_LEFT, 220, 0);

        // rain probability column - show percentage for rain
        if (fe.precipitationType == "rain" && fe.precipitationProbability > 0.0f) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%.0f%%", fe.precipitationProbability * 100);
            lv_obj_t *rain_label = lv_label_create(row);
            lv_label_set_text(rain_label, buf);
            lv_obj_set_style_text_color(rain_label, Theme::accentRain(), 0);
            lv_obj_align(rain_label, LV_ALIGN_TOP_LEFT, 260, 0);
        }
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
