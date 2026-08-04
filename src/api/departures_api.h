// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#pragma once
#include <Arduino.h>
#include <vector>

struct Departure {
    String station;
    String type;
    String icon;
    String line;
    String destination;
    long timeEpoch = 0;
    String timeStr;
    int delayMin = 0;
    bool cancelled = false;
};

struct DeparturesData {
    bool valid = false;
    std::vector<Departure> departures;
};

DeparturesData fetchDepartures();
