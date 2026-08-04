// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#pragma once
#include <Arduino.h>
#include "api/weather_api.h"
#include "api/calendar_api.h"

// Centralized async data manager for weather + calendar.
//
// A background FreeRTOS task fetches weather and calendar data every 10
// minutes (or on manual trigger). Results are cached in memory and exposed
// to the UI screens via thread-safe getters.
//
// Screens poll dataManager_dataVersion() in their tick_fn to detect changes
// and re-render from cache. Navigation between screens never blocks on a
// fetch — views always render instantly from whatever is cached.

// Create the background task and trigger the initial fetch. Call once in setup().
void dataManager_begin();

// No-op for now (background task runs independently). Reserved for future use.
void dataManager_tick();

// Request a manual refresh (non-blocking). The background task picks this up
// within ~1 second.
void dataManager_triggerRefresh();

// Thread-safe copy of the cached weather/calendar data into `out`.
void dataManager_getWeather(WeatherData &out);
void dataManager_getCalendar(CalendarData &out);

// Status flags (safe to read from the main loop without a mutex).
bool dataManager_isWeatherOk();
bool dataManager_isCalendarOk();
bool dataManager_isLoading();
uint32_t dataManager_dataVersion();
unsigned long dataManager_lastFetchMs();
