// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#include "departures_api.h"
#include "http_client.h"
#include <secrets.h>
#include <ArduinoJson.h>

DeparturesData fetchDepartures() {
    DeparturesData data;
    ApiResult res = httpGet(DEPARTURES_API_URL);
    if (!res.success) { data.valid = false; return data; }

    DynamicJsonDocument doc(16384);
    if (deserializeJson(doc, res.body)) { data.valid = false; return data; }

    JsonArray departures = doc["departures"];
    for (JsonObject d : departures) {
        Departure dep;
        dep.station = d["station"] | "";
        dep.type = d["type"] | "";
        dep.icon = d["icon"] | "";
        dep.line = d["line"] | "";
        dep.destination = d["destination"] | "";
        dep.timeEpoch = d["time_epoch"] | 0;
        dep.timeStr = d["time_str"] | "";
        dep.delayMin = d["delay_min"] | 0;
        dep.cancelled = d["cancelled"] | false;
        data.departures.push_back(dep);
    }
    data.valid = true;
    return data;
}
