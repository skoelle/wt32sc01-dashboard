#pragma once
#include <Arduino.h>

// No NTP/network time used, purely computed from date strings coming from
// the calendar API itself, as specified.
namespace DateUtils {

inline const char *weekdayShortDE(int isoWeekday /* 0=Mon..6=Sun */) {
    static const char *names[7] = {"Mo", "Di", "Mi", "Do", "Fr", "Sa", "So"};
    if (isoWeekday < 0 || isoWeekday > 6) return "??";
    return names[isoWeekday];
}

// Sakamoto's algorithm, returns 0=Monday..6=Sunday
inline int computeWeekdayMonBased(int year, int month, int day) {
    static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    int y = year;
    if (month < 3) y -= 1;
    int w = (y + y / 4 - y / 100 + y / 400 + t[month - 1] + day) % 7; // 0=Sun
    return (w + 6) % 7; // convert to 0=Mon
}

struct ParsedDateTime {
    bool valid = false;
    int year = 0, month = 0, day = 0, hour = 0, minute = 0;
};

inline ParsedDateTime parseIso(const String &iso) {
    ParsedDateTime r;
    if (iso.length() < 10) return r;

    int y = iso.substring(0, 4).toInt();
    int mo = iso.substring(5, 7).toInt();
    int d = iso.substring(8, 10).toInt();

    int h = 0, mi = 0;
    int tIdx = iso.indexOf('T');
    if (tIdx > 0 && iso.length() >= tIdx + 6) {
        h = iso.substring(tIdx + 1, tIdx + 3).toInt();
        mi = iso.substring(tIdx + 4, tIdx + 6).toInt();
    }

    if (y == 0 || mo == 0 || d == 0) return r;

    r.valid = true;
    r.year = y; r.month = mo; r.day = d; r.hour = h; r.minute = mi;
    return r;
}

// "Mo 3.8. 14:00" for timed events, "Mo 3.8." for all-day events.
inline String formatShortDE(const String &iso, bool allDay) {
    ParsedDateTime p = parseIso(iso);
    if (!p.valid) return iso;

    int wd = computeWeekdayMonBased(p.year, p.month, p.day);
    String out = String(weekdayShortDE(wd)) + " " + String(p.day) + "." + String(p.month) + ".";

    if (!allDay) {
        char buf[8];
        snprintf(buf, sizeof(buf), " %02d:%02d", p.hour, p.minute);
        out += buf;
    }
    return out;
}

// Date-only column text, e.g. "Mo 3.8." (used in the calendar detail table).
inline String formatDateDE(const String &iso) {
    ParsedDateTime p = parseIso(iso);
    if (!p.valid) return iso;
    int wd = computeWeekdayMonBased(p.year, p.month, p.day);
    return String(weekdayShortDE(wd)) + " " + String(p.day) + "." + String(p.month) + ".";
}

// Time-only column text, e.g. "14:00". Returns "-" for all-day events so the
// table row keeps a consistent column structure.
inline String formatTimeDE(const String &iso, bool allDay) {
    if (allDay) return "";
    ParsedDateTime p = parseIso(iso);
    if (!p.valid) return "";
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", p.hour, p.minute);
    return String(buf);
}

} // namespace DateUtils
