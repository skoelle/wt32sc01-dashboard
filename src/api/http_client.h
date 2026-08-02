#pragma once
#include <Arduino.h>

struct ApiResult {
    bool success = false;
    String body;
    int httpCode = -1;
};

ApiResult httpGet(const String &url, uint32_t timeoutMs = 5000);
