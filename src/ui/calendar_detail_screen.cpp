// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#include "calendar_detail_screen.h"
#include "ui/widgets/back_button.h"
#include "data/data_manager.h"
#include <theme.h>
#include <text_utils.h>
#include <date_utils.h>
#include <lvgl.h>

namespace {

ScreenId (*g_navigate)(ScreenId) = nullptr;
void on_back(lv_event_t *) { if (g_navigate) g_navigate(ScreenId::HOME); }

lv_obj_t *list = nullptr;

void buildList() {
    if (!list) return;
    lv_obj_clean(list);

    CalendarData c;
    dataManager_getCalendar(c);
    bool ok = dataManager_isCalendarOk();
    bool loading = dataManager_isLoading();

    if (!ok) {
        lv_obj_t *lbl = lv_label_create(list);
        lv_label_set_text(lbl, loading ? LV_SYMBOL_REFRESH " Lade..."
                                       : LV_SYMBOL_WARNING " Keine Verbindung");
        lv_obj_set_style_text_color(lbl, loading ? Theme::textDim() : Theme::accentError(), 0);
        return;
    }

    for (const auto &ev : c.events) {
        lv_obj_t *row = lv_obj_create(list);
        lv_obj_set_size(row, 280, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 1, 0);
        lv_obj_set_style_border_side(row, LV_BORDER_SIDE_BOTTOM, 0);
        lv_obj_set_style_border_color(row, Theme::bgCard(), 0);
        lv_obj_set_style_pad_all(row, 8, 0);
        lv_obj_set_style_pad_bottom(row, 12, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        // Zeile 1: Datum + Uhrzeit
        lv_obj_t *date = lv_label_create(row);
        lv_label_set_text(date, DateUtils::formatDateDE(ev.startAt).c_str());
        lv_obj_set_style_text_color(date, Theme::text(), 0);
        lv_obj_set_style_text_font(date, &lv_font_montserrat_24, 0);
        lv_obj_align(date, LV_ALIGN_TOP_LEFT, 0, 0);

        lv_obj_t *time = lv_label_create(row);
        lv_label_set_text(time, DateUtils::formatTimeDE(ev.startAt, ev.allDay).c_str());
        lv_obj_set_style_text_color(time, Theme::text(), 0);
        lv_obj_set_style_text_font(time, &lv_font_montserrat_24, 0);
        lv_obj_align_to(time, date, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

        // Zeile 2: Kalendereintrag
        lv_obj_t *sum = lv_label_create(row);
        lv_label_set_text(sum, sanitizeGermanText(ev.summary.substring(0, 30)).c_str());
        lv_obj_set_style_text_color(sum, Theme::textDim(), 0);
        lv_obj_set_style_text_font(sum, &lv_font_montserrat_20, 0);
        lv_obj_align(sum, LV_ALIGN_TOP_LEFT, 0, 28);
    }
}

} // namespace

void calendarDetailScreen_setNavigator(ScreenId (*nav)(ScreenId)) { g_navigate = nav; }

void calendarDetailScreen_create(Screen &s) {
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

void calendarDetailScreen_refresh(Screen &) {
    buildList();
}

void calendarDetailScreen_tick(Screen &) {
    static uint32_t lastVersion = 0;
    uint32_t v = dataManager_dataVersion();
    if (v != lastVersion) {
        lastVersion = v;
        buildList();
    }
}

Screen calendarDetailScreen_make() {
    Screen s;
    s.create_fn  = calendarDetailScreen_create;
    s.refresh_fn = calendarDetailScreen_refresh;
    s.tick_fn    = calendarDetailScreen_tick;
    return s;
}
