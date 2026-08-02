#include "http_client.h"
#include <HTTPClient.h>
#include <WiFi.h>

ApiResult httpGet(const String &url, uint32_t timeoutMs) {
    ApiResult result;
    if (WiFi.status() != WL_CONNECTED) {
        result.success = false;
        result.httpCode = -1;
        return result;
    }
    HTTPClient http;
    http.setTimeout(timeoutMs);
    http.begin(url);
    int code = http.GET();
    result.httpCode = code;
    if (code == HTTP_CODE_OK) {
        result.body = http.getString();
        result.success = true;
    } else {
        result.success = false;
    }
    http.end();
    return result;
}
