// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#include "calendar_api.h"
#include "http_client.h"
#include <secrets.h>
#include <ArduinoJson.h>

CalendarData fetchCalendar() {
    CalendarData data;
    ApiResult res = httpGet(CALENDAR_API_URL);
    if (!res.success) { data.valid = false; return data; }

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, res.body)) { data.valid = false; return data; }

    JsonArray events = doc["events"];
    for (JsonObject e : events) {
        CalendarEvent ev;
        ev.id = e["id"] | 0;
        ev.summary = e["summary"] | "";
        ev.startAt = e["start_at"] | "";
        ev.endAt = e["end_at"] | "";
        ev.allDay = e["all_day"] | false;
        ev.status = e["status"] | "";
        data.events.push_back(ev);
    }
    data.valid = true;
    return data;
}
