#include "weather_api.h"
#include "http_client.h"
#include <secrets.h>
#include <ArduinoJson.h>

WeatherData fetchWeather() {
    WeatherData data;
    ApiResult res = httpGet(WEATHER_API_URL);
    if (!res.success) { data.valid = false; return data; }

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, res.body)) { data.valid = false; return data; }

    JsonObject current = doc["current"];
    data.current.temperature = current["temperature"] | 0;
    data.current.symbol = current["symbol"] | "";
    data.current.description = current["description"] | "";

    JsonArray forecast = doc["forecast"];
    for (JsonObject entry : forecast) {
        ForecastEntry fe;
        fe.time = entry["time"] | "";
        fe.temperature = entry["temperature"] | 0;
        fe.symbol = entry["symbol"] | "";
        fe.description = entry["description"] | "";
        JsonObject precip = entry["precipitation"];
        fe.precipitationProbability = precip["probability"] | 0.0f;
        fe.precipitationType = precip["type"] | "";
        data.forecast.push_back(fe);
    }
    data.willRainSoon = doc["willRainSoon"] | false;
    data.valid = true;
    return data;
}
