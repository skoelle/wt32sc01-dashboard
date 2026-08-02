#include "calendar_detail_screen.h"
#include "ui/widgets/back_button.h"
#include "api/calendar_api.h"
#include <theme.h>
#include <text_utils.h>
#include <date_utils.h>
#include <lvgl.h>

namespace {

ScreenId (*g_navigate)(ScreenId) = nullptr;
void on_back(lv_event_t *) { if (g_navigate) g_navigate(ScreenId::HOME); }

CalendarData lastCalendar;
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

    // Columns (content width = 280 - 2*6 pad = 268), side by side like a table:
    //   date   72 px left (dim grey)  | time 56 px centered (grey) | summary (white, wraps)
    for (const auto &ev : lastCalendar.events) {
        lv_obj_t *row = lv_obj_create(list);
        lv_obj_set_size(row, 280, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 1, LV_PART_MAIN);
        lv_obj_set_style_border_side(row, LV_BORDER_SIDE_BOTTOM, 0);
        lv_obj_set_style_border_color(row, Theme::bgCard(), 0);
        lv_obj_set_style_pad_all(row, 6, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        // Shared base: ensure no default-black text leaks through to children.
        lv_obj_set_style_text_color(row, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

        lv_obj_t *date = lv_label_create(row);
        lv_label_set_text(date, DateUtils::formatDateDE(ev.startAt).c_str());
        lv_obj_set_style_text_color(date, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_width(date, 72);
        lv_obj_align(date, LV_ALIGN_TOP_LEFT, 0, 0);

        lv_obj_t *time = lv_label_create(row);
        lv_label_set_text(time, DateUtils::formatTimeDE(ev.startAt, ev.allDay).c_str());
        lv_obj_set_style_text_color(time, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_text_align(time, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(time, 56);
        lv_obj_align(time, LV_ALIGN_TOP_LEFT, 80, 0);

        lv_obj_t *sum = lv_label_create(row);
        lv_label_set_text(sum, sanitizeGermanText(ev.summary.substring(0, 30)).c_str());
        lv_obj_set_style_text_color(sum, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_label_set_long_mode(sum, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_width(sum, 124);
        lv_obj_set_height(sum, LV_SIZE_CONTENT);
        lv_obj_align(sum, LV_ALIGN_TOP_LEFT, 144, 0);
    }
}

} // namespace

void calendarDetailScreen_setNavigator(ScreenId (*nav)(ScreenId)) { g_navigate = nav; }

void calendarDetailScreen_create(Screen &s) {
    list = lv_list_create(s.root);
    lv_obj_set_pos(list, 0, 0);
    lv_obj_set_size(list, 320, 480 - 80);
    lv_obj_set_style_bg_color(list, Theme::bg(), 0);
    lv_obj_set_style_border_width(list, 0, 0);
    back_button_create(s.root, on_back, nullptr);
}

void calendarDetailScreen_refresh(Screen &) {
    CalendarData c = fetchCalendar();
    ok = c.valid;
    if (c.valid) lastCalendar = c;
    buildList();
}

void calendarDetailScreen_tick(Screen &) {}

Screen calendarDetailScreen_make() {
    Screen s;
    s.create_fn  = calendarDetailScreen_create;
    s.refresh_fn = calendarDetailScreen_refresh;
    s.tick_fn    = calendarDetailScreen_tick;
    return s;
}
