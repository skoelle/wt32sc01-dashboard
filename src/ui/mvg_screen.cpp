#include "mvg_screen.h"
#include "ui/widgets/back_button.h"
#include "api/departures_api.h"
#include <theme.h>
#include <text_utils.h>
#include <lvgl.h>

namespace {

ScreenId (*g_navigate)(ScreenId) = nullptr;
void on_back(lv_event_t *) { if (g_navigate) g_navigate(ScreenId::HOME); }

DeparturesData lastData;
bool ok = false;
lv_obj_t *list = nullptr;
unsigned long lastFetchMs = 0;
const unsigned long REFRESH_INTERVAL_MS = 60UL * 1000UL;

lv_color_t typeColor(const String &type) {
    String t = type;
    t.toUpperCase();
    if (t.indexOf("U") >= 0 && t.indexOf("BAHN") >= 0) return Theme::accentUBahn();
    if (t.indexOf("S") >= 0 && t.indexOf("BAHN") >= 0) return Theme::accentSBahn();
    return Theme::textDim();
}

void buildList() {
    if (!list) return;
    lv_obj_clean(list);

    if (!ok) {
        lv_obj_t *lbl = lv_label_create(list);
        lv_label_set_text(lbl, LV_SYMBOL_WARNING " Keine Verbindung");
        lv_obj_set_style_text_color(lbl, Theme::accentError(), 0);
        return;
    }

    for (const auto &dep : lastData.departures) {
        lv_obj_t *row = lv_obj_create(list);
        lv_obj_set_size(row, 280, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_all(row, 6, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_text_color(row, Theme::text(), 0);

        String badge = String(dep.icon) + " " + dep.line;
        lv_obj_t *ln = lv_label_create(row);
        lv_label_set_text(ln, badge.c_str());
        lv_obj_set_style_text_color(ln, typeColor(dep.type), 0);
        lv_obj_align(ln, LV_ALIGN_TOP_LEFT, 0, 0);

        String dest = sanitizeGermanText(dep.destination.substring(0, 20));
        lv_obj_t *d = lv_label_create(row);
        lv_label_set_text(d, dest.c_str());
        lv_obj_set_style_text_color(d, lv_color_hex(0xFFFFFF), 0);
        lv_obj_align(d, LV_ALIGN_TOP_LEFT, 0, 18);

        String right;
        if (dep.cancelled) {
            right = "Ausfall";
        } else {
            right = dep.timeStr;
            if (dep.delayMin > 0) right += " +" + String(dep.delayMin);
        }
        lv_obj_t *t = lv_label_create(row);
        lv_label_set_text(t, right.c_str());
        lv_obj_set_style_text_color(t,
            dep.cancelled ? Theme::accentError() : Theme::text(), 0);
        lv_obj_align(t, LV_ALIGN_TOP_RIGHT, 0, 18);
    }
}

void doFetch() {
    DeparturesData d = fetchDepartures();
    ok = d.valid;
    if (d.valid) lastData = d;
    lastFetchMs = millis();
}

} // namespace

void mvgScreen_setNavigator(ScreenId (*nav)(ScreenId)) { g_navigate = nav; }

void mvgScreen_create(Screen &s) {
    list = lv_obj_create(s.root);
    lv_obj_set_pos(list, 0, 0);
    lv_obj_set_size(list, 320, 480 - 80);
    lv_obj_set_style_bg_color(list, Theme::bg(), 0);
    lv_obj_set_style_bg_opa(list, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_style_text_color(list, Theme::text(), 0);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLLABLE);
    back_button_create(s.root, on_back, nullptr);
}

void mvgScreen_refresh(Screen &) {
    doFetch();
    buildList();
}

void mvgScreen_tick(Screen &) {
    if (lastFetchMs == 0 || millis() - lastFetchMs >= REFRESH_INTERVAL_MS) {
        doFetch();
        buildList();
    }
}

Screen mvgScreen_make() {
    Screen s;
    s.create_fn  = mvgScreen_create;
    s.refresh_fn = mvgScreen_refresh;
    s.tick_fn    = mvgScreen_tick;
    return s;
}
