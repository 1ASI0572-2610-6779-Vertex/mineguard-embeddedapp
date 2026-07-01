#include "EdgeTelemetryClient.h"
#include <HTTPClient.h>
#include <WiFi.h>

EdgeTelemetryClient::EdgeTelemetryClient(const char* baseUrl, const char* deviceId, const char* apiKey)
    : telemetryUrl(String(baseUrl) + "/api/v1/telemetry"),
      deviceId(deviceId),
      apiKey(apiKey) {
}

String EdgeTelemetryClient::buildPayload(double latitude, double longitude, float distanceCm,
                                         int bpm, bool collision) const {
    String payload = "{";
    payload += "\"device_id\":\"" + deviceId + "\",";
    payload += "\"bpm\":" + String(bpm) + ",";
    payload += "\"distance_cm\":" + String(distanceCm, 1) + ",";
    payload += "\"collision\":" + String(collision ? "true" : "false");

    if (latitude != 0.0 || longitude != 0.0) {
        payload += ",\"lat\":" + String(latitude, 6);
        payload += ",\"lng\":" + String(longitude, 6);
    }

    payload += "}";
    return payload;
}

bool EdgeTelemetryClient::send(double latitude, double longitude, float distanceCm, int bpm, bool collision) const {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Edge telemetry skipped: WiFi is disconnected.");
        return false;
    }

    HTTPClient http;
    http.setTimeout(5000);

    if (!http.begin(telemetryUrl)) {
        Serial.println("Edge telemetry failed: invalid URL.");
        return false;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-Key", apiKey);

    String payload = buildPayload(latitude, longitude, distanceCm, bpm, collision);
    int responseCode = http.POST(payload);

    if (responseCode == 201) {
        Serial.println("Edge telemetry sent.");
        http.end();
        return true;
    }

    Serial.print("Edge telemetry rejected. HTTP ");
    Serial.print(responseCode);
    Serial.print(": ");
    Serial.println(http.getString());
    http.end();
    return false;
}
