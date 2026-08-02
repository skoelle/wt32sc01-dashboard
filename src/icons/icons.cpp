#include "icons.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Icons {

// Helper: create a transparent canvas of the given square size.
static lv_obj_t *makeCanvas(lv_obj_t *parent, int size) {
    lv_obj_t *c = lv_canvas_create(parent);
    lv_coord_t w = size, h = size;
    // Allocate draw buffer in PSRAM (ARGB8888 = 32 bpp, stride 0).
    uint32_t buf_size = lv_canvas_buf_size(w, h, 32, 1);
    void *buf = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    lv_canvas_set_buffer(c, buf, w, h, LV_COLOR_FORMAT_ARGB8888);
    lv_obj_set_size(c, w, h);
    lv_canvas_fill_bg(c, lv_color_make(0, 0, 0), LV_OPA_TRANSP);
    return c;
}

static void drawCircle(lv_obj_t *canvas, int cx, int cy, int r, lv_color_t color) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = color;
    dsc.bg_opa = LV_OPA_COVER;
    dsc.border_width = 0;

    lv_area_t a;
    a.x1 = cx - r; a.y1 = cy - r;
    a.x2 = cx + r; a.y2 = cy + r;
    lv_draw_rect(&layer, &dsc, &a);

    lv_canvas_finish_layer(canvas, &layer);
}

static void fillRect(lv_obj_t *canvas, int x, int y, int w, int h, lv_color_t color) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = 0;
    dsc.bg_color = color;
    dsc.bg_opa = LV_OPA_COVER;
    dsc.border_width = 0;

    lv_area_t a;
    a.x1 = x; a.y1 = y;
    a.x2 = x + w - 1; a.y2 = y + h - 1;
    lv_draw_rect(&layer, &dsc, &a);

    lv_canvas_finish_layer(canvas, &layer);
}

static void drawLine(lv_obj_t *canvas, int x1, int y1, int x2, int y2,
                     lv_color_t color, int width) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_line_dsc_t dsc;
    lv_draw_line_dsc_init(&dsc);
    dsc.color = color;
    dsc.width = width;
    dsc.opa = LV_OPA_COVER;
    dsc.round_end = 1;
    dsc.round_start = 1;
    dsc.p1.x = x1; dsc.p1.y = y1;
    dsc.p2.x = x2; dsc.p2.y = y2;

    lv_draw_line(&layer, &dsc);

    lv_canvas_finish_layer(canvas, &layer);
}

// Cloud composed of three overlapping circles + a filler rectangle.
static void drawCloud(lv_obj_t *canvas, int cx, int cy, int scale, lv_color_t color) {
    drawCircle(canvas, cx - scale, cy, scale, color);
    drawCircle(canvas, cx + scale, cy, scale, color);
    drawCircle(canvas, cx, cy - scale / 2, scale + 2, color);
    fillRect(canvas, cx - scale, cy, scale * 2, scale, color);
}

lv_obj_t *createWeatherIcon(lv_obj_t *parent, const String &symbol,
                            const String &description, int size) {
    lv_obj_t *c = makeCanvas(parent, size);
    bool isNight = symbol.length() > 0 && symbol.charAt(0) == 'm';
    char condition = symbol.length() > 1 ? symbol.charAt(1) : 'o';
    bool isThunder = (condition == 't') ||
                     (description.indexOf("Gewitter") >= 0);

    int cx = size / 2;
    int cy = size / 2;
    int r = size / 4;

    if (isThunder) {
        drawCloud(c, cx, cy - r / 2, r * 2 / 3, Theme::textDim());
        drawLine(c, cx, cy + r / 2, cx - r / 3, cy + r, Theme::accentStorm(), 3);
        drawLine(c, cx - r / 3, cy + r, cx + r / 4, cy + r + r / 2, Theme::accentStorm(), 3);
        return c;
    }

    switch (condition) {
        case 'o': // clear
            if (isNight) {
                drawCircle(c, cx, cy, r, Theme::textDim());
                drawCircle(c, cx + r / 2, cy - r / 3, r, lv_color_hex(0x000000));
            } else {
                drawCircle(c, cx, cy, r, Theme::accentSun());
                for (int i = 0; i < 8; ++i) {
                    float a = i * (M_PI / 4.0f);
                    int x1 = cx + (int)(cosf(a) * (r + 2));
                    int y1 = cy + (int)(sinf(a) * (r + 2));
                    int x2 = cx + (int)(cosf(a) * (r + 6));
                    int y2 = cy + (int)(sinf(a) * (r + 6));
                    drawLine(c, x1, y1, x2, y2, Theme::accentSun(), 2);
                }
            }
            break;
        case 'b': // cloudy
            drawCloud(c, cx, cy, r * 2 / 3, Theme::textDim());
            break;
        case 'r': // rain
            drawCloud(c, cx, cy - 3, r * 2 / 3, Theme::textDim());
            for (int i = -1; i <= 1; ++i) {
                drawLine(c, cx + i * 6, cy + r / 2,
                         cx + i * 6 - 2, cy + r, Theme::accentRain(), 2);
            }
            break;
        default:
            drawCloud(c, cx, cy, r * 2 / 3, Theme::textDim());
            break;
    }
    return c;
}

lv_obj_t *createRainWarning(lv_obj_t *parent, int size) {
    lv_obj_t *c = makeCanvas(parent, size);
    int cx = size / 2;
    // Triangle outline via three lines.
    int top = 2, bottom = size - 2;
    int left = 4, right = size - 4;
    drawLine(c, cx, top, left, bottom, Theme::accentRain(), 2);
    drawLine(c, left, bottom, right, bottom, Theme::accentRain(), 2);
    drawLine(c, right, bottom, cx, top, Theme::accentRain(), 2);
    // Exclamation dot.
    fillRect(c, cx - 1, cx - 2, 2, 6, Theme::accentRain());
    fillRect(c, cx - 1, bottom - 4, 2, 2, Theme::accentRain());
    return c;
}

lv_obj_t *createRetryIcon(lv_obj_t *parent, int size) {
    lv_obj_t *c = makeCanvas(parent, size);
    int cx = size / 2;
    int r = size / 2 - 2;
    // Circle outline (drawn as thick ring via 4 arcs of lines).
    for (int i = 0; i < 24; ++i) {
        float a = i * (2 * M_PI / 24);
        int x = cx + (int)(cosf(a) * r);
        int y = cx + (int)(sinf(a) * r);
        fillRect(c, x - 1, y - 1, 2, 2, Theme::accentError());
    }
    // Exclamation mark.
    fillRect(c, cx - 1, cx - 4, 2, 6, Theme::accentError());
    fillRect(c, cx - 1, cx + 3, 2, 2, Theme::accentError());
    return c;
}

lv_obj_t *createBackArrow(lv_obj_t *parent, int size) {
    lv_obj_t *c = makeCanvas(parent, size);
    int cx = size / 2;
    int cy = size / 2;
    int len = size / 3;
    // Arrow head.
    drawLine(c, cx - len, cy, cx, cy - len, Theme::text(), 3);
    drawLine(c, cx - len, cy, cx, cy + len, Theme::text(), 3);
    // Shaft.
    drawLine(c, cx - len, cy, cx + len, cy, Theme::text(), 3);
    return c;
}

} // namespace Icons
