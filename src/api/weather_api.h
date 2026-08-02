#pragma once
#include <Arduino.h>
#include <vector>

struct WeatherCurrent {
    int temperature = 0;
    String symbol;
    String description;
};

struct ForecastEntry {
    String time;
    int temperature = 0;
    String symbol;
    String description;
    float precipitationProbability = 0.0f;
    String precipitationType;
};

struct WeatherData {
    bool valid = false;
    WeatherCurrent current;
    std::vector<ForecastEntry> forecast;
};

WeatherData fetchWeather();
bool willRainSoon(const WeatherData &data, int hours = 8);
