/**
 * @file mineguard-embeddedapp.ino
 * @brief MineGuard embedded application.
 *
 * Architecture:
 *   - The Arduino loop is intentionally idle.
 *   - Collision and SOS inputs are captured by hardware interrupts.
 *   - Distance, heart rate, and GPS are sampled by a FreeRTOS worker task.
 *   - LCD, LEDs, and buzzer refresh only when the state changes.
 */

#include "MineGuardDevice.h"
#include "EdgeTelemetryClient.h"
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>

static const char* WIFI_SSID = "YOUR_WIFI_SSID";
static const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Use the IP address of the computer or gateway running the Flask edge service.
// Do not use localhost or 127.0.0.1 from the ESP32.
static const char* EDGE_BASE_URL = "http://192.168.1.100:5001";
static const char* EDGE_DEVICE_ID = "smart-band-001";
static const char* EDGE_API_KEY = "test-api-key-123";

MineGuardDevice device;
EdgeTelemetryClient edgeClient(EDGE_BASE_URL, EDGE_DEVICE_ID, EDGE_API_KEY);

void connectWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());
}

void sendTelemetryToEdge(double latitude, double longitude, float distanceCm, int bpm,
                         bool proximityAlert, bool collisionAlert, bool sosAlert) {
    (void)proximityAlert;
    (void)sosAlert;
    edgeClient.send(latitude, longitude, distanceCm, bpm, collisionAlert);
}

void samplingTask(void* parameter) {
    while (true) {
        device.update();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22); // I2C: SDA=21, SCL=22

    Serial.println("MineGuard embedded application starting...");
    connectWiFi();
    device.initialize();
    device.setMetricCallback(sendTelemetryToEdge);

    xTaskCreatePinnedToCore(
        samplingTask,
        "MineGuardSampling",
        8192,
        nullptr,
        1,
        nullptr,
        1
    );

    Serial.println("MineGuard ready. Runtime is task/event driven.");
}

void loop() {
    // Intentionally idle: runtime work is handled by interrupts and timer ticks.
}
