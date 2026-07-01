#ifndef MINEGUARD_DEVICE_H
#define MINEGUARD_DEVICE_H

/**
 * @file MineGuardDevice.h
 * @brief Application mediator for the MineGuard embedded safety node.
 *
 * Owns the hardware adapters, reacts to sensor events, applies the safety
 * policy, refreshes local indicators, and exposes a telemetry callback for an
 * external delivery layer.
 */

#include <Arduino.h>
#include "MineGuardEvent.h"
#include "UltrasonicSensor.h"
#include "PulseSensor.h"
#include "CollisionSensor.h"
#include "EmergencyButton.h"
#include "GPSSensor.h"
#include "Led.h"
#include "Buzzer.h"
#include "LcdDisplay.h"
#include "SafetyPolicy.h"

class MineGuardDevice : public MineGuardEventSink {
private:
    // -- Actuators --
    Led         dangerLed;
    Led         safeLed;
    Buzzer      buzzer;
    LcdDisplay  display;

    // -- Sensors --
    UltrasonicSensor proximity;
    PulseSensor      pulse;
    CollisionSensor  collision;
    EmergencyButton  sos;
    GPSSensor        gps;
    SafetyPolicy     safetyPolicy;

    // -- Latched alert windows (momentary events held for visibility) --
    unsigned long collisionHoldUntil;
    unsigned long sosHoldUntil;

    // -- Telemetry throttling --
    unsigned long lastMetricSentAt;

    // -- Reactive change-tracking (render/actuate only on change) --
    String        lastSignature;
    bool          lastDanger;
    bool          indicatorsInitialized;

    // -- Edge Service callback: (lat, lng, minDistanceCm, bpm, proximity, collision, sos) --
    void (*metricCallback)(double, double, float, int, bool, bool, bool);

    void refreshIndicators(bool danger);
    String buildHeader(const MineGuardStatus& status);
    String buildDataLine(const MineGuardStatus& status, int bpm, float distanceCm);

public:
    // -- Actuator / I2C / serial pins --
    static const int RED_LED_PIN    = 15;
    static const int GREEN_LED_PIN  = 2;
    static const int BUZZER_PIN     = 23;
    static const uint8_t LCD_ADDRESS = 0x27;

    // -- Ultrasonic sensor (TRIG / ECHO) --
    static const int DISTANCE_TRIG_PIN = 5;
    static const int DISTANCE_ECHO_PIN = 18;

    // -- Other sensors --
    static const int PULSE_PIN      = 32;
    static const int COLLISION_PIN  = 4;
    static const int SOS_PIN        = 19;
    static const int GPS_SERIAL     = 2;  // UART2
    static const int GPS_RX_PIN     = 16;
    static const int GPS_TX_PIN     = 17;

    // -- Behaviour timings --
    static const unsigned long COLLISION_HOLD_MS = 3000;
    static const unsigned long SOS_HOLD_MS       = 5000;
    static const unsigned long TELEMETRY_INTERVAL_MS = 5000;

    MineGuardDevice();

    // -- Event contract --
    void notify(MineGuardEvent event) override;

    // -- Application lifecycle --
    bool initialize();
    void update();
    void setMetricCallback(void (*callback)(double, double, float, int, bool, bool, bool));

    // -- Accessors --
    Led& getDangerLed();
    Led& getSafeLed();
    Buzzer& getBuzzer();
    LcdDisplay& getDisplay();
    UltrasonicSensor& getDistanceSensor();
    PulseSensor& getPulseSensor();
    CollisionSensor& getCollisionSensor();
    EmergencyButton& getEmergencyButton();
    GPSSensor& getGPSSensor();
};

#endif // MINEGUARD_DEVICE_H
