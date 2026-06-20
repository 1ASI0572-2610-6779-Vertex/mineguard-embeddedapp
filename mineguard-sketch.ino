/**
 * @file mineguard-sketch.ino
 * @brief MineGuard IoT node — runtime canvas (WiFi + Edge Service + sensing loop).
 *
 * Boots WiFi, authenticates the device against the MineGuard Edge Service
 * (register, then validate on later boots), initializes the MineGuardDevice,
 * and on every loop scans the sensors, drives local alerts and streams safety
 * telemetry to the backend.
 *
 * Wokwi libraries needed: ArduinoJson, TinyGPSPlus, LiquidCrystal_I2C.
 * (WiFi, HTTPClient, Preferences and Wire ship with the ESP32 core.)
 */

#include "MineGuardDevice.h"
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

MineGuardDevice device;
Preferences prefs;
String apiToken = "";

// ── Device identity ───────────────────────────────────────
String deviceId   = "mineguard-iot-001";
const int minerId = 1;

// ── WiFi configuration ────────────────────────────────────
// In Wokwi use "Wokwi-GUEST" with empty password (channel 6).
// On real hardware replace with your network credentials.
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ── Edge Service endpoints (replace host/IP with your backend) ──
const char* edgeServiceUrl   = "http://192.168.18.7:5000/api/v1/metrics";
const char* validationUrl    = "http://192.168.18.7:5000/api/v1/devices/authentication/validate";
const char* authenticationUrl = "http://192.168.18.7:5000/api/v1/devices/authentication/register";

void saveApiKey(const String& apiKey) {
    prefs.begin("auth", false);
    prefs.putString("apiKey", apiKey);
    prefs.end();
}

String loadApiKey() {
    prefs.begin("auth", true);
    String key = prefs.getString("apiKey", "");
    prefs.end();
    return key;
}

void connectWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password, 6); // channel 6 speeds up Wokwi association
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

bool registerDevice() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return false;
    }
    HTTPClient http;
    http.setTimeout(5000);
    if (!http.begin(authenticationUrl)) {
        Serial.println("Failed to connect to authentication server");
        return false;
    }
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<256> doc;
    doc["deviceId"] = deviceId;
    doc["minerId"]  = minerId;
    String jsonString;
    serializeJson(doc, jsonString);

    Serial.println("Registering device...");
    int code = http.POST(jsonString);

    if (code == 200) {
        String response = http.getString();
        StaticJsonDocument<512> responseDoc;
        if (deserializeJson(responseDoc, response)) {
            Serial.println("JSON parsing failed");
            http.end();
            return false;
        }
        apiToken = responseDoc["token"].as<String>();
        if (apiToken.isEmpty()) {
            Serial.println("Token not found in registration response");
            http.end();
            return false;
        }
        saveApiKey(apiToken);
        Serial.println("Device registered. Token stored.");
        http.end();
        return true;
    }
    Serial.print("Registration failed, code: ");
    Serial.println(code);
    Serial.println(http.getString());
    http.end();
    return false;
}

bool getTokenFromValidation() {
    if (WiFi.status() != WL_CONNECTED) return false;
    HTTPClient http;
    http.setTimeout(5000);
    if (!http.begin(validationUrl)) return false;
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<128> doc;
    doc["deviceId"] = deviceId;
    String jsonString;
    serializeJson(doc, jsonString);

    Serial.println("Validating stored token...");
    int code = http.POST(jsonString);

    if (code == 200) {
        String response = http.getString();
        StaticJsonDocument<512> responseDoc;
        if (deserializeJson(responseDoc, response)) {
            http.end();
            return false;
        }
        apiToken = responseDoc["token"].as<String>();
        if (apiToken.isEmpty()) {
            http.end();
            return false;
        }
        saveApiKey(apiToken);
        Serial.println("Token validated.");
        http.end();
        return true;
    }
    Serial.print("Validation failed, code: ");
    Serial.println(code);
    http.end();
    return false;
}

bool authenticateDevice() {
    apiToken = loadApiKey();
    if (!apiToken.isEmpty()) {
        Serial.println("Found stored API key, validating...");
        if (getTokenFromValidation()) return true;
        Serial.println("Stored API key invalid, re-registering...");
    }
    Serial.println("Registering device for the first time...");
    return registerDevice();
}

// ── Telemetry callback injected into the device ──
void sendMetric(double latitude, double longitude, float distanceCm, int heartRateBpm,
                bool proximityAlert, bool collisionDetected, bool sosActive) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi down, reconnecting...");
        connectWiFi();
        return;
    }
    if (apiToken.isEmpty()) {
        Serial.println("No API token, cannot send metric.");
        return;
    }

    HTTPClient http;
    http.setTimeout(5000);
    if (!http.begin(edgeServiceUrl)) {
        Serial.println("Failed to begin HTTP connection");
        return;
    }
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + apiToken);

    StaticJsonDocument<512> doc;
    doc["minerId"]            = minerId;
    doc["latitude"]           = latitude;
    doc["longitude"]          = longitude;
    doc["distanceCm"]         = distanceCm;
    doc["heartRateBpm"]       = heartRateBpm;
    doc["proximityAlert"]     = proximityAlert;
    doc["collisionDetected"]  = collisionDetected;
    doc["sosActive"]          = sosActive;

    String jsonString;
    serializeJson(doc, jsonString);

    Serial.println("Sending metric to Edge Service...");
    int code = http.POST(jsonString);
    if (code > 0) {
        Serial.print("Response code: ");
        Serial.println(code);
        Serial.print("Response: ");
        Serial.println(http.getString());
    } else {
        Serial.print("Error sending metric: ");
        Serial.println(code);
    }
    http.end();
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    Wire.begin(21, 22); // I2C: SDA=21, SCL=22

    Serial.println("MineGuard IoT Device Starting");

    connectWiFi();

    if (!authenticateDevice()) {
        Serial.println("Authentication failed. Retrying...");
        while (!authenticateDevice()) {
            delay(5000);
            Serial.println("Retrying authentication...");
        }
    }

    if (!device.initialize()) {
        Serial.println("Device initialization failed!");
        while (1) delay(1000);
    }
    device.setMetricCallback(sendMetric);

    Serial.println("MineGuard ready.");
}

void loop() {
    device.update();
    delay(150); // responsive local alerts; telemetry is throttled inside update()
}
