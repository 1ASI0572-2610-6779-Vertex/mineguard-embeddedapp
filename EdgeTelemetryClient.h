#ifndef EDGE_TELEMETRY_CLIENT_H
#define EDGE_TELEMETRY_CLIENT_H

/**
 * @file EdgeTelemetryClient.h
 * @brief HTTP client that sends MineGuard telemetry to the edge service.
 */

#include <Arduino.h>

class EdgeTelemetryClient {
private:
    String telemetryUrl;
    String deviceId;
    String apiKey;

    String buildPayload(double latitude, double longitude, float distanceCm, int bpm, bool collision) const;

public:
    /**
     * @brief Creates an edge telemetry client.
     * @param baseUrl Edge base URL, for example http://192.168.1.20:5001.
     * @param deviceId Registered edge device id.
     * @param apiKey API key paired with the device id.
     */
    EdgeTelemetryClient(const char* baseUrl, const char* deviceId, const char* apiKey);

    /**
     * @brief Sends one telemetry record to POST /api/v1/telemetry.
     * @return True when the edge responds with HTTP 201.
     */
    bool send(double latitude, double longitude, float distanceCm, int bpm, bool collision) const;
};

#endif // EDGE_TELEMETRY_CLIENT_H
