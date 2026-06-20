#include "MineGuardDevice.h"
#include <Arduino.h>

MineGuardDevice::MineGuardDevice(
    int redLedPin, int greenLedPin, int buzzerPin, uint8_t lcdAddr,
    int trigPin, int echoPin, int pulsePin, int collisionPin, int sosPin,
    int gpsSerial, int gpsRx, int gpsTx)
    : dangerLed(redLedPin, false, this),
      safeLed(greenLedPin, true, this),
      buzzer(buzzerPin, 1000, this),
      display(lcdAddr, 16, 2, this),
      proximity(trigPin, echoPin, this),
      pulse(pulsePin, this),
      collision(collisionPin, this),
      sos(sosPin, this),
      gps(gpsSerial, gpsRx, gpsTx, this),
      collisionHoldUntil(0),
      sosHoldUntil(0),
      lastMetricSentAt(0),
      metricPending(false),
      metricCallback(nullptr) {
}

bool MineGuardDevice::initialize() {
    Serial.println("Initializing MineGuard Device...");
    if (!display.initialize()) {
        Serial.println("Failed to initialize LCD display!");
        return false;
    }
    display.setLine(0, " MineGuard V2.1 ");
    display.setLine(1, "  Initializing..");
    display.handle(LcdDisplay::REFRESH_COMMAND);
    Serial.println("MineGuard Device initialized successfully!");
    return true;
}

void MineGuardDevice::setMetricCallback(void (*callback)(double, double, float, int, bool, bool, bool)) {
    metricCallback = callback;
}

// ── Event-driven reactions: log, latch momentary events, flag telemetry ──
void MineGuardDevice::on(Event event) {
    if (event == UltrasonicSensor::OBJECT_NEARBY_EVENT) {
        Serial.print("OBJECT NEARBY: ");
        Serial.print(proximity.getDistance());
        Serial.println(" cm");
        metricPending = true;

    } else if (event == PulseSensor::HIGH_HEART_RATE_EVENT) {
        Serial.print("HIGH HEART RATE: ");
        Serial.print(pulse.getBpm());
        Serial.println(" bpm");
        metricPending = true;

    } else if (event == PulseSensor::FATIGUE_EVENT) {
        Serial.print("FATIGUE (low heart rate): ");
        Serial.print(pulse.getBpm());
        Serial.println(" bpm");
        metricPending = true;

    } else if (event == CollisionSensor::COLLISION_DETECTED_EVENT) {
        Serial.println("COLLISION DETECTED!");
        collisionHoldUntil = millis() + COLLISION_HOLD_MS;
        metricPending = true;

    } else if (event == EmergencyButton::SOS_PRESSED_EVENT) {
        Serial.println("SOS ACTIVATED by operator!");
        sosHoldUntil = millis() + SOS_HOLD_MS;
        metricPending = true;
    }
}

// ── Optional command tracing ──
void MineGuardDevice::handle(Command command) {
    if (command == Led::TURN_ON_COMMAND || command == Led::TURN_OFF_COMMAND ||
        command == Buzzer::TURN_ON_COMMAND || command == Buzzer::TURN_OFF_COMMAND) {
        // Hook for actuator-state logging if needed.
    }
}

void MineGuardDevice::refreshIndicators(bool danger) {
    if (danger) {
        dangerLed.handle(Led::TURN_ON_COMMAND);
        safeLed.handle(Led::TURN_OFF_COMMAND);
        buzzer.handle(Buzzer::TURN_ON_COMMAND);
    } else {
        dangerLed.handle(Led::TURN_OFF_COMMAND);
        safeLed.handle(Led::TURN_ON_COMMAND);
        buzzer.handle(Buzzer::TURN_OFF_COMMAND);
    }
}

void MineGuardDevice::refreshDisplay(bool proximityAlert, bool hrAlert,
                                     bool collisionActive, bool sosActive, int bpm) {
    // Row 0: highest-priority alert headline
    if (sosActive)            display.setLine(0, "!SOS ACTIVATED! ");
    else if (collisionActive) display.setLine(0, "!COLLISION ALERT");
    else if (hrAlert && bpm < PulseSensor::FATIGUE_BPM_THRESHOLD)
                              display.setLine(0, "!FATIGUE ALERT! ");
    else if (proximityAlert)  display.setLine(0, "!OBJECT NEARBY! ");
    else if (hrAlert)         display.setLine(0, "!HIGH HEART RATE");
    else                      display.setLine(0, "MineGuard [OK]  ");

    // Row 1: GPS while SOS is active, otherwise live sensor readings
    char buffer[17];
    if (sosActive) {
        if (gps.getLatitude() != 0.0 || gps.getLongitude() != 0.0) {
            snprintf(buffer, sizeof(buffer), "G:%2.2f,%2.2f",
                     gps.getLatitude(), gps.getLongitude());
        } else {
            snprintf(buffer, sizeof(buffer), "GPS: Searching..");
        }
    } else {
        int d = (int)proximity.getDistance();
        snprintf(buffer, sizeof(buffer), "D:%3dcm P:%3dbpm", d, bpm);
    }
    display.setLine(1, String(buffer));
    display.handle(LcdDisplay::REFRESH_COMMAND);
}

// ── Main application tick: scan sensors, drive actuators, push telemetry ──
void MineGuardDevice::update() {
    // 1. Scan every sensor (each may raise events via on())
    proximity.scanDistance();
    pulse.scanPulse();
    collision.scanCollision();
    sos.scanButton();
    gps.scanLocation();

    // 2. Compute current alert states
    int bpm = pulse.getBpm();
    bool proximityAlert = (proximity.getDistance() <= UltrasonicSensor::PROXIMITY_THRESHOLD_CM);
    bool hrAlert        = (bpm >= PulseSensor::HIGH_BPM_THRESHOLD ||
                           bpm <  PulseSensor::FATIGUE_BPM_THRESHOLD);
    bool collisionActive = (millis() < collisionHoldUntil);
    bool sosActive       = (millis() < sosHoldUntil);
    bool danger = proximityAlert || hrAlert || collisionActive || sosActive;

    // 3. Drive local indicators and the LCD
    refreshIndicators(danger);
    refreshDisplay(proximityAlert, hrAlert, collisionActive, sosActive, bpm);

    // 4. Console snapshot
    Serial.println("\n=== MINEGUARD READINGS ===");
    Serial.print("Distance: ");   Serial.print(proximity.getDistance()); Serial.println(" cm");
    Serial.print("Heart Rate: "); Serial.print(bpm);                     Serial.println(" bpm");
    Serial.print("Collision: ");  Serial.println(collisionActive ? "YES" : "NO");
    Serial.print("SOS: ");        Serial.println(sosActive ? "ACTIVE" : "idle");
    Serial.print("Location: ");   Serial.print(gps.getLatitude(), 6);
    Serial.print(", ");           Serial.println(gps.getLongitude(), 6);
    Serial.println("==========================\n");

    // 5. Throttled telemetry to the Edge Service
    if (metricPending && metricCallback != nullptr &&
        (millis() - lastMetricSentAt >= METRIC_MIN_INTERVAL_MS)) {
        metricCallback(
            gps.getLatitude(),
            gps.getLongitude(),
            proximity.getDistance(),
            bpm,
            proximityAlert,
            collisionActive,
            sosActive
        );
        lastMetricSentAt = millis();
        metricPending = false;
    }
}

// ── Accessors ──
Led& MineGuardDevice::getDangerLed() { return dangerLed; }
Led& MineGuardDevice::getSafeLed() { return safeLed; }
Buzzer& MineGuardDevice::getBuzzer() { return buzzer; }
LcdDisplay& MineGuardDevice::getDisplay() { return display; }
UltrasonicSensor& MineGuardDevice::getProximitySensor() { return proximity; }
PulseSensor& MineGuardDevice::getPulseSensor() { return pulse; }
CollisionSensor& MineGuardDevice::getCollisionSensor() { return collision; }
EmergencyButton& MineGuardDevice::getEmergencyButton() { return sos; }
GPSSensor& MineGuardDevice::getGPSSensor() { return gps; }
