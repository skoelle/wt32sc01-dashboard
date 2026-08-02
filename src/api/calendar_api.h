#pragma once
#include <Arduino.h>
#include <vector>

struct CalendarEvent {
    long id = 0;
    String summary;
    String startAt;
    String endAt;
    bool allDay = false;
    String status;
};

struct CalendarData {
    bool valid = false;
    std::vector<CalendarEvent> events;
};

CalendarData fetchCalendar();
