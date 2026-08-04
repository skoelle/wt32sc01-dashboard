// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#include "weather_detail_screen.h"
#include "ui/widgets/back_button.h"
#include "data/data_manager.h"
#include "icons/icons.h"
#include <theme.h>
#include <text_utils.h>
#include <lvgl.h>

namespace {

ScreenId (*g_navigate)(ScreenId) = nullptr;
void on_back(lv_event_t *) { if (g_navigate) g_navigate(ScreenId::HOME); }

lv_obj_t *list = nullptr;

void buildList() {
    if (!list) return;
    lv_obj_clean(list);

    WeatherData w;
    dataManager_getWeather(w);
    bool ok = dataManager_isWeatherOk();
    bool loading = dataManager_isLoading();

    if (!ok) {
        lv_obj_t *lbl = lv_label_create(list);
        lv_label_set_text(lbl, loading ? LV_SYMBOL_REFRESH " Lade..."
                                       : LV_SYMBOL_WARNING " Keine Verbindung");
        lv_obj_set_style_text_color(lbl, loading ? Theme::textDim() : Theme::accentError(), 0);
        return;
    }

    for (const auto &fe : w.forecast) {
        lv_obj_t *row = lv_obj_create(list);
        lv_obj_set_size(row, 280, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 1, 0);
        lv_obj_set_style_border_side(row, LV_BORDER_SIDE_BOTTOM, 0);
        lv_obj_set_style_border_color(row, Theme::bgCard(), 0);
        lv_obj_set_style_pad_all(row, 8, 0);
        lv_obj_set_style_pad_bottom(row, 12, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        int tIdx = fe.time.indexOf('T');
        String hhmm = tIdx > 0 ? fe.time.substring(tIdx + 1, tIdx + 6) : fe.time;

        // Zeile 1: Icon + Uhrzeit + Grad + Regen%
        lv_obj_t *icon = Icons::createWeatherIcon(row, fe.symbol, fe.description, 24);
        lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 0, 0);

        lv_obj_t *time_label = lv_label_create(row);
        lv_label_set_text(time_label, hhmm.c_str());
        lv_obj_set_style_text_color(time_label, Theme::text(), 0);
        lv_obj_set_style_text_font(time_label, &lv_font_montserrat_24, 0);
        lv_obj_align_to(time_label, icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

        lv_obj_t *temp_label = lv_label_create(row);
        lv_label_set_text(temp_label, (String(fe.temperature) + "°").c_str());
        lv_obj_set_style_text_color(temp_label, Theme::text(), 0);
        lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_24, 0);
        lv_obj_align_to(temp_label, time_label, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

        if (fe.precipitationType == "rain" && fe.precipitationProbability > 0.0f) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%.0f%%", fe.precipitationProbability * 100);
            lv_obj_t *rain_label = lv_label_create(row);
            lv_label_set_text(rain_label, buf);
            lv_obj_set_style_text_color(rain_label, Theme::accentRain(), 0);
            lv_obj_set_style_text_font(rain_label, &lv_font_montserrat_24, 0);
            lv_obj_align_to(rain_label, temp_label, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
        }

        // Zeile 2: Beschreibung (kleiner)
        String desc = sanitizeGermanText(fe.description.substring(0, 25));
        lv_obj_t *desc_label = lv_label_create(row);
        lv_label_set_text(desc_label, desc.c_str());
        lv_obj_set_style_text_color(desc_label, Theme::textDim(), 0);
        lv_obj_set_style_text_font(desc_label, &lv_font_montserrat_20, 0);
        lv_obj_align(desc_label, LV_ALIGN_TOP_LEFT, 0, 28);
    }
}

} // namespace

void weatherDetailScreen_setNavigator(ScreenId (*nav)(ScreenId)) { g_navigate = nav; }

void weatherDetailScreen_create(Screen &s) {
    list = lv_obj_create(s.root);
    lv_obj_set_pos(list, 0, 0);
    lv_obj_set_size(list, 320, 480 - 80);
    lv_obj_set_style_bg_color(list, Theme::bg(), 0);
    lv_obj_set_style_bg_opa(list, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 0, 0);
    lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLLABLE);
    back_button_create(s.root, on_back, nullptr);
}

void weatherDetailScreen_refresh(Screen &) {
    buildList();
}

void weatherDetailScreen_tick(Screen &) {
    static uint32_t lastVersion = 0;
    uint32_t v = dataManager_dataVersion();
    if (v != lastVersion) {
        lastVersion = v;
        buildList();
    }
}

Screen weatherDetailScreen_make() {
    Screen s;
    s.create_fn  = weatherDetailScreen_create;
    s.refresh_fn = weatherDetailScreen_refresh;
    s.tick_fn    = weatherDetailScreen_tick;
    return s;
}
