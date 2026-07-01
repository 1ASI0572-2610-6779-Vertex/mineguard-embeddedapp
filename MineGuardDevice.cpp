#include "MineGuardDevice.h"
#include <Arduino.h>

MineGuardDevice::MineGuardDevice()
    : dangerLed(RED_LED_PIN, false),
      safeLed(GREEN_LED_PIN, true),
      buzzer(BUZZER_PIN, 450),
      display(LCD_ADDRESS, 16, 2),
      proximity(DISTANCE_TRIG_PIN, DISTANCE_ECHO_PIN, this),
      pulse(PULSE_PIN, this),
      collision(COLLISION_PIN, this),
      sos(SOS_PIN, this),
      gps(GPS_SERIAL, GPS_RX_PIN, GPS_TX_PIN),
      collisionHoldUntil(0),
      sosHoldUntil(0),
      lastMetricSentAt(0),
      lastSignature(""),
      lastDanger(false),
      indicatorsInitialized(false),
      metricCallback(nullptr) {
}

bool MineGuardDevice::initialize() {
    Serial.println("Initializing MineGuard embedded device...");
    if (!display.initialize()) {
        Serial.println("Failed to initialize LCD display!");
        return false;
    }
    display.setLine(0, " MineGuard Ready");
    display.setLine(1, "Sensors starting");
    display.refresh();
    Serial.println("MineGuard ready. Inputs are event-driven and timer-sampled.");
    return true;
}

void MineGuardDevice::setMetricCallback(void (*callback)(double, double, float, int, bool, bool, bool)) {
    metricCallback = callback;
}

// Event reactions: latch momentary events and flag telemetry.
void MineGuardDevice::notify(MineGuardEvent event) {
    if (event == MineGuardEvent::ObjectNearby) {
        Serial.println(">> EVENT: OBJECT NEARBY");
    } else if (event == MineGuardEvent::HighHeartRate) {
        Serial.println(">> EVENT: HIGH HEART RATE");
    } else if (event == MineGuardEvent::Fatigue) {
        Serial.println(">> EVENT: FATIGUE");
    } else if (event == MineGuardEvent::CollisionDetected) {
        Serial.println(">> EVENT: COLLISION DETECTED (interrupt)");
        collisionHoldUntil = millis() + COLLISION_HOLD_MS;
    } else if (event == MineGuardEvent::SosPressed) {
        Serial.println(">> EVENT: SOS PRESSED (interrupt)");
        sosHoldUntil = millis() + SOS_HOLD_MS;
    }
}

void MineGuardDevice::refreshIndicators(bool danger) {
    if (danger) {
        dangerLed.handle(LedCommand::TurnOn);
        safeLed.handle(LedCommand::TurnOff);
        buzzer.handle(BuzzerCommand::TurnOn);
    } else {
        dangerLed.handle(LedCommand::TurnOff);
        safeLed.handle(LedCommand::TurnOn);
        buzzer.handle(BuzzerCommand::TurnOff);
    }
}

String MineGuardDevice::buildHeader(const MineGuardStatus& status) {
    char header[17];
    if (status.sosAlert) {
        snprintf(header, sizeof(header), "!SOS ACTIVATED! ");
    } else if (status.collisionAlert) {
        snprintf(header, sizeof(header), "!COLLISION ALERT");
    } else if (status.proximityAlert) {
        snprintf(header, sizeof(header), "!NEAR OBJECT!  ");
    } else if (status.highHeartRateAlert) {
        snprintf(header, sizeof(header), "!HIGH HEART RATE");
    } else if (status.fatigueAlert) {
        snprintf(header, sizeof(header), "!FATIGUE ALERT! ");
    } else {
        snprintf(header, sizeof(header), "MineGuard [OK]  ");
    }
    return String(header);
}

String MineGuardDevice::buildDataLine(const MineGuardStatus& status, int bpm, float distanceCm) {
    char buffer[17];
    if (status.sosAlert) {
        if (gps.hasFix()) {
            snprintf(buffer, sizeof(buffer), "G:%2.2f,%2.2f",
                     gps.getLatitude(), gps.getLongitude());
        } else {
            snprintf(buffer, sizeof(buffer), "GPS: Searching..");
        }
    } else {
        if (bpm > 0) {
            snprintf(buffer, sizeof(buffer), "D:%3dcm P:%3dbpm", (int)distanceCm, bpm);
        } else {
            snprintf(buffer, sizeof(buffer), "D:%3dcm P:--- ", (int)distanceCm);
        }
    }
    return String(buffer);
}

// Sampling tick: reads sensors, consumes interrupt-captured events, and
// refreshes outputs only when the visible state changes.
void MineGuardDevice::update() {
    proximity.scanDistance();
    pulse.scanPulse();
    gps.scanLocation();

    collision.poll();
    sos.poll();

    MineGuardSnapshot snapshot = {
        proximity.getDistance(),
        pulse.getBpm(),
        pulse.hasSignal(),
        millis() < collisionHoldUntil,
        millis() < sosHoldUntil
    };
    MineGuardStatus status = safetyPolicy.evaluate(snapshot);

    if (status.danger != lastDanger || !indicatorsInitialized) {
        refreshIndicators(status.danger);
        lastDanger = status.danger;
        indicatorsInitialized = true;
    }

    String header = buildHeader(status);
    String dataLine = buildDataLine(status, snapshot.bpm, snapshot.distanceCm);
    String signature = header + "|" + dataLine + (status.danger ? "|D" : "|S");

    if (signature != lastSignature) {
        display.setLine(0, header);
        display.setLine(1, dataLine);
        display.refresh();

        Serial.print("[CHANGE] ");
        Serial.print(header); Serial.print(" | "); Serial.print(dataLine);
        Serial.print("  (D:"); Serial.print((int)snapshot.distanceCm);
        Serial.print("cm P:"); Serial.print(snapshot.bpm);
        Serial.print("bpm GPS:"); Serial.print(gps.getLatitude(), 5);
        Serial.print(","); Serial.print(gps.getLongitude(), 5);
        Serial.println(")");

        lastSignature = signature;
    }

    if (metricCallback != nullptr && (millis() - lastMetricSentAt >= TELEMETRY_INTERVAL_MS)) {
        metricCallback(gps.getLatitude(), gps.getLongitude(), snapshot.distanceCm, snapshot.bpm,
                       status.proximityAlert, status.collisionAlert, status.sosAlert);
        lastMetricSentAt = millis();
    }
}

Led& MineGuardDevice::getDangerLed() { return dangerLed; }
Led& MineGuardDevice::getSafeLed() { return safeLed; }
Buzzer& MineGuardDevice::getBuzzer() { return buzzer; }
LcdDisplay& MineGuardDevice::getDisplay() { return display; }
UltrasonicSensor& MineGuardDevice::getDistanceSensor() { return proximity; }
PulseSensor& MineGuardDevice::getPulseSensor() { return pulse; }
CollisionSensor& MineGuardDevice::getCollisionSensor() { return collision; }
EmergencyButton& MineGuardDevice::getEmergencyButton() { return sos; }
GPSSensor& MineGuardDevice::getGPSSensor() { return gps; }
