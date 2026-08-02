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

    for (const auto &ev : lastCalendar.events) {
        lv_obj_t *row = lv_obj_create(list);
        lv_obj_set_size(row, 280, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_all(row, 6, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *sum = lv_label_create(row);
        lv_label_set_text(sum, sanitizeGermanText(ev.summary.substring(0, 30)).c_str());
        lv_obj_set_style_text_color(sum, Theme::text(), 0);
        lv_obj_align(sum, LV_ALIGN_TOP_LEFT, 0, 0);

        lv_obj_t *when = lv_label_create(row);
        lv_label_set_text(when, DateUtils::formatShortDE(ev.startAt, ev.allDay).c_str());
        lv_obj_set_style_text_color(when, Theme::accentCalendar(), 0);
        lv_obj_align(when, LV_ALIGN_TOP_LEFT, 0, 20);
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
